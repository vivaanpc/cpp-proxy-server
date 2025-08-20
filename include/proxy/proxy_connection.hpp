#pragma once
//PriyanshuSingh
#include "../common.hpp"
namespace proxy
{

     //bidirectional data transfer between client and target server
     //error handling and connection management
    class ProxyConnection : public std::enable_shared_from_this<ProxyConnection>
    {
    public:
        ProxyConnection(boost::asio::io_context &io_context, tcp::socket client_socket,
            const tcp::endpoint &client_endpoint);

        tcp::socket &client_socket() { return client_socket_; }
        tcp::socket &server_socket() { return server_socket_; }
        const tcp::endpoint &client_endpoint() const { return client_endpoint_; }

        void connect_to_server(const std::string &host, const std::string &port,
            std::function<void(const error_code &)> callback);

        void start_relay();

        void send_to_client(const std::string &response, std::function<void(const error_code &, 
            size_t)> callback = nullptr);

        void send_to_server(const std::string &request,
            std::function<void(const error_code &, size_t)> callback = nullptr);

        void read_from_client(std::function<void(const error_code &, const std::string &)> callback);

        void close();

    private:
        boost::asio::io_context &io_context_;
        tcp::socket client_socket_;
        tcp::socket server_socket_;
        tcp::endpoint client_endpoint_;

        std::array<char, BUFFER_SIZE> client_buffer_;
        std::array<char, BUFFER_SIZE> server_buffer_;

        void relay_client_to_server();

        void relay_server_to_client();

        void handle_client_read(const error_code &ec, size_t bytes_transferred);

        void handle_server_read(const error_code &ec, size_t bytes_transferred);
    };

}