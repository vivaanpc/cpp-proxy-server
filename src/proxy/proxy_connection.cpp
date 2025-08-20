//PriyanshuSingh
#include "proxy/proxy_connection.hpp"
#include "utils/logger.hpp"
#include <boost/asio/connect.hpp>
//PriyanshuSingh done 
namespace proxy {
    ProxyConnection::ProxyConnection(boost::asio::io_context& io_context, tcp::socket client_socket,
            const tcp::endpoint& client_endpoint): io_context_(io_context),
        client_socket_(std::move(client_socket)),
        server_socket_(io_context),
        client_endpoint_(client_endpoint) {
    }
void ProxyConnection::connect_to_server(const std::string& host, const std::string& port,
            std::function<void(const error_code&)> callback) {

        // Keep resolver alive until async ops finish
        auto resolver = std::make_shared<tcp::resolver>(io_context_);

        auto self = shared_from_this();
        resolver->async_resolve(host, port,
        [self, callback, resolver](const error_code & ec, tcp::resolver::results_type endpoints) {
            if (ec) {
                callback(ec);
                return;
            }

            boost::asio::async_connect(self->server_socket_, endpoints,
            [callback, resolver](const error_code & ec, const tcp::endpoint&) {
                // 'resolver' captured here keeps it alive until connect completes
                callback(ec);
            });
        });
    }


    void ProxyConnection::start_relay() {
        auto self = shared_from_this();
        relay_client_to_server();
        relay_server_to_client();
    }

//completed vivaanpc
    void ProxyConnection::send_to_client(const std::string& response,
        std::function<void(const error_code&, size_t)> callback) {
        auto self = shared_from_this();

        boost::asio::async_write(client_socket_, boost::asio::buffer(response),
        [self, callback](const error_code & ec, size_t bytes_transferred) {
            if (callback) {
                callback(ec, bytes_transferred);
            }
        });
    }
void ProxyConnection::send_to_server(const std::string& request,
            std::function<void(const error_code&, size_t)> callback) {
        auto self = shared_from_this();

        boost::asio::async_write(server_socket_, boost::asio::buffer(request),
        [self, callback](const error_code & ec, size_t bytes_transferred) {
            if (callback) {
                callback(ec, bytes_transferred);
            }
        });
    }

//complted PriyanshuSingh
    void ProxyConnection::read_from_client(std::function<void(const error_code&, const std::string&)> callback) {
        auto self = shared_from_this();

        client_socket_.async_read_some(boost::asio::buffer(client_buffer_),
        [self, callback](const error_code & ec, size_t bytes_transferred) {
            if (ec) {
                callback(ec, "");
                return;
            }

            std::string data(self->client_buffer_.data(), bytes_transferred);
            callback(error_code(), data);
        });
    }


    void ProxyConnection::close() {
        error_code ec;
        client_socket_.close(ec);
        server_socket_.close(ec);
    }


    void ProxyConnection::relay_client_to_server() {
        auto self = shared_from_this();

        client_socket_.async_read_some(boost::asio::buffer(client_buffer_),
        [self](const error_code & ec, size_t bytes_transferred) {
            self->handle_client_read(ec, bytes_transferred);
        });
    }


    void ProxyConnection::relay_server_to_client() {
        auto self = shared_from_this();

        server_socket_.async_read_some(boost::asio::buffer(server_buffer_),
        [self](const error_code & ec, size_t bytes_transferred) {
            self->handle_server_read(ec, bytes_transferred);
        });
    }

//completed vivaanpc
    void ProxyConnection::handle_client_read(const error_code& ec, size_t bytes_transferred) {
        if (ec) {
            if (ec != boost::asio::error::eof) {
                Logger::log_debug("Client read error: " + ec.message());
            }
            close();
            return;
        }

        auto self = shared_from_this();
        boost::asio::async_write(server_socket_,
                                boost::asio::buffer(client_buffer_.data(), bytes_transferred),
        [self](const error_code & ec, size_t) {
            if (ec) {
                Logger::log_debug("Server write error: " + ec.message());
                self->close();
                return;
            }

            // Continue relaying
            self->relay_client_to_server();
        });
    }

//completed PriyanshuSingh == YES 
    void ProxyConnection::handle_server_read(const error_code& ec, size_t bytes_transferred) {
        if (ec) {
            if (ec != boost::asio::error::eof) {
                Logger::log_debug("Server read error: " + ec.message());
            }
            close();
            return;
        }

        auto self = shared_from_this();
        boost::asio::async_write(client_socket_,
                                boost::asio::buffer(server_buffer_.data(), bytes_transferred),
        [self](const error_code & ec, size_t) {
            if (ec) {
                Logger::log_debug("Client write error: " + ec.message());
                self->close();
                return;
            }
            self->relay_server_to_client();
        });
    }

}