#pragma once
//vivaanpc
#include "../common.hpp"
#include "proxy_connection.hpp"
// Handles HTTP and HTTPS (CONNECT) requests by forwarding them
// to the target servers specified in the request.
namespace proxy
{
    class ForwardProxy
    {
    public:

        explicit ForwardProxy(boost::asio::io_context &io_context, const Config &config);
        void handle_connection(tcp::socket client_socket, const tcp::endpoint &client_endpoint);

    private:

        boost::asio::io_context &io_context_;
        const Config &config_;
        void handle_http_request(std::shared_ptr<ProxyConnection> connection, const HttpRequest &request);
        void handle_connect_request(std::shared_ptr<ProxyConnection> connection, const HttpRequest &request);
        // host,port pairing
        std::pair<std::string, std::string> parse_host_port(const std::string &uri);
    
    };

}