// Copyright 2021 by FORTYSS

#include <server.hpp>
#include "suggestion.hpp"

int main(int argc, char* argv[]) {
  try
  {
    if(argc != 3) {
      std::cerr << "Usage: " << argv[0] << " <address> <port>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 80\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 80\n";
      return EXIT_FAILURE;
    }

    auto const address = boost::asio::ip::make_address(argv[1]);
    unsigned short port = static_cast<unsigned short>(std::atoi(argv[2]));

    boost::asio::io_context ioc{1};

    suggestion sug = suggestion("/home/FORTYSS/Рабочий стол/1/7-8/"
        "lab-http-server-07/suggestions.json");

    std::thread sug_thread([&sug]{
      sug.update_suggestion();
    });
    sug_thread.detach();

    boost::asio::ip::tcp::acceptor acceptor{ioc, {address, port}};
    boost::asio::ip::tcp::socket socket{ioc};
    server(acceptor, socket, sug);

    ioc.run();
  } catch(std::exception const& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}