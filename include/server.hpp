// Copyright 2021 by FORTYSS


#ifndef INCLUDE_SERVER_HPP_
#define INCLUDE_SERVER_HPP_
#include <iostream>
#include <string>
#include <memory>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <utility>
#include <cstdlib>
#include <ctime>
#include <suggestion.hpp>

class connection : public std::enable_shared_from_this<connection> {
 public:
  explicit connection(boost::asio::ip::tcp::socket socket, suggestion& sug)
      : socket_(std::move(socket)), sug_(sug) {}

  // Инициирую асинхронные операции, связанные с подключением.
  void start();

 private:
  //Сокет для текущего подключенного клиента.
  boost::asio::ip::tcp::socket socket_;

  suggestion& sug_;

  //Буфер для выполнения операций чтения.
  boost::beast::flat_buffer buffer_{8192};

  // Сообщение с запросом.
  boost::beast::http::request<boost::beast::http::string_body> request_;

  // Ответное сообщение.
  boost::beast::http::response<boost::beast::http::string_body> response_;

  // Таймер для установки крайнего срока обработки соединения.
  boost::asio::steady_timer deadline_{
      socket_.get_executor(), std::chrono::seconds(60)};

  // Асинхронно получаю полное сообщение запроса.
  void read_request();

  // Определяю, что нужно сделать с сообщением запроса.
  void process_request();

  // Составляю ответное сообщение на основе состояния программы.
  void create_response();

  // Асинхронно передаваю ответное сообщение.
  void write_response();

  // Проверяю, достаточно ли времени я потратил на это соединение.
  void check_deadline();
};

void server(boost::asio::ip::tcp::acceptor& acceptor,
                 boost::asio::ip::tcp::socket& socket, suggestion& sug);
#endif // INCLUDE_SERVER_HPP_
