//vivaanpc
#pragma once
#include "../common.hpp"
#include "../proxy/forward_proxy.hpp"
#include "../proxy/reverse_proxy.hpp"
namespace proxy {
 //Handles client connections based on proxy mode
class ClientHandler {
    public:

    ClientHandler(boost::asio::io_context& io_context, const Config& config);
    void handle_connection(tcp::socket client_socket, const tcp::endpoint& client_endpoint);

    private:
        boost::asio::io_context& io_context_;
        const Config& config_;
        std::unique_ptr<ForwardProxy> forward_proxy_;
        std::unique_ptr<ReverseProxy> reverse_proxy_;
    };

}