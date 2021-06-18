// Copyright 2021 by FORTYSS

#include <server.hpp>

void connection::start() {
  read_request();
  check_deadline();
}
void connection::read_request() {
  auto self = shared_from_this();

  boost::beast::http::async_read(
      socket_, buffer_, request_,
      [self](boost::beast::error_code ec,
             std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if (!ec)
          self->process_request();
      });
}
void connection::process_request() {
  response_.version(request_.version());
  response_.keep_alive(false);

  switch (request_.method())
  {
    case boost::beast::http::verb::post:
      response_.result(boost::beast::http::status::ok);
      response_.set(boost::beast::http::field::server, "Beast");
      create_response();
      break;

    default:
      // We return responses indicating an error if
      // we do not recognize the request method.
      response_.result(boost::beast::http::status::bad_request);
      response_.set(boost::beast::http::field::content_type,
                    "text/plain");
      response_.body() = "Invalid request-method '"
          + std::string(request_.method_string()) + "'";
      break;
  }

  write_response();
}
void connection::create_response() {
  if (request_.target() == "/v1/api/suggest")
  {
    response_.set(boost::beast::http::field::content_type,
                  "application/json");
    response_.body() = sug_.get_json_suggestion(
                               request_.body()).dump(4);
  } else {
    response_.result(boost::beast::http::status::not_found);
    response_.set(boost::beast::http::field::content_type,
                  "text/plain");
    response_.body() = "File not found\r\n";
  }
}
void connection::write_response() {
  auto self = shared_from_this();

  response_.content_length(response_.body().size());

  boost::beast::http::async_write(socket_, response_,
                    [self](boost::beast::error_code ec, std::size_t) {
                      self->socket_.shutdown(boost::asio::ip::tcp::socket::
                                                               shutdown_send,
                                                           ec);
                      self->deadline_.cancel();
                    });
}
void connection::check_deadline() {
  auto self = shared_from_this();

  deadline_.async_wait([self](boost::beast::error_code ec) {
    if (!ec) {
      // Закрываю сокет, чтобы отменить любую невыполненную операцию.
      self->socket_.close(ec);
    }
  });
}
void server(boost::asio::ip::tcp::acceptor& acceptor,
                 boost::asio::ip::tcp::socket& socket, suggestion& sug){
  acceptor.async_accept(socket, [&](boost::beast::error_code ec) {
    if (!ec)
      std::make_shared<connection>(std::move(socket), sug)->start();
    server(acceptor, socket, sug);
  });
}
