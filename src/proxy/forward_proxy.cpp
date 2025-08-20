//vivaanpc
#include "proxy/forward_proxy.hpp"
#include "utils/http_parser.hpp"
#include "utils/logger.hpp"
namespace proxy
{

    ForwardProxy::ForwardProxy(boost::asio::io_context &io_context, const Config &config)
        : io_context_(io_context), config_(config)
    {
    }

    void ForwardProxy::handle_connection(tcp::socket client_socket, const tcp::endpoint &client_endpoint)
    {
        auto connection = std::make_shared<ProxyConnection>(io_context_, std::move(client_socket), client_endpoint);

        std::string client_ip = client_endpoint.address().to_string();

        Logger::log_info("New forward proxy connection from " + client_ip);

        // initial request
        connection->read_from_client([this, connection, client_ip](const error_code &ec, const std::string &data) {
            if (ec) {
                Logger::log_error("Failed to read from client: " + ec.message(), client_ip);
                return;
            }

            try {
                if (!HttpParser::is_complete_request(data)) {
                    Logger::log_error("Incomplete request received", client_ip);
                    return;
                }

                HttpRequest request = HttpParser::parse_request(data);
                Logger::log_request(client_ip, request.method, request.uri);

                if (request.method == "CONNECT") {
                    handle_connect_request(connection, request);
                } else {
                    handle_http_request(connection, request);
                }
            } catch (const std::exception& e) {
                Logger::log_error("Failed to parse request: " + std::string(e.what()), client_ip);
        } });
    }


    void ForwardProxy::handle_http_request(std::shared_ptr<ProxyConnection> connection, const HttpRequest &request)
    {
        std::string client_ip = connection->client_endpoint().address().to_string();

        auto [host, port] = parse_host_port(request.uri);
        if (host.empty())
        {
            std::string error_response = "HTTP/1.1 400 Bad Request\r\n\r\n";
            connection->send_to_client(error_response);
            Logger::log_response(client_ip, 400);
            return;
        }

        Logger::log_connection(client_ip, host, port);

        connection->connect_to_server(host, port, [this, connection, request, client_ip, host, port](const error_code &ec)
                                      {
        if (ec) {
            Logger::log_error("Failed to connect to server " + host + ":" + port + ": " + ec.message(), client_ip);
            std::string error_response = "HTTP/1.1 502 Bad Gateway\r\n\r\n";
            connection->send_to_client(error_response);
            Logger::log_response(client_ip, 502);
            return;
        }

        //Forwarding
        std::string request_str = request.to_string();
        connection->send_to_server(request_str, [connection, client_ip](const error_code& ec, size_t) {
            if (ec) {
                Logger::log_error("Failed to send request to server: " + ec.message(), client_ip);
                return;
            }
            connection->start_relay();
        }); });
    }

    //completed? PriyanshuSingh
    void ForwardProxy::handle_connect_request(std::shared_ptr<ProxyConnection> connection, const HttpRequest &request)
    {
        std::string client_ip = connection->client_endpoint().address().to_string();

        auto [host, port] = HttpParser::extract_host_port(request.uri);
        if (host.empty())
        {
            std::string error_response = "HTTP/1.1 400 Bad Request\r\n\r\n";
            connection->send_to_client(error_response);
            Logger::log_response(client_ip, 400);
            return;
        }

        Logger::log_connection(client_ip, host, port);

        connection->connect_to_server(host, port, [connection, client_ip](const error_code &ec)
                                      {
        if (ec) {
            Logger::log_error("Failed to establish CONNECT tunnel: " + ec.message(), client_ip);
            std::string error_response = "HTTP/1.1 502 Bad Gateway\r\n\r\n";
            connection->send_to_client(error_response);
            Logger::log_response(client_ip, 502);
            return;
        }

        // success response
        std::string success_response = "HTTP/1.1 200 Connection established\r\n\r\n";
        connection->send_to_client(success_response, [connection, client_ip](const error_code& ec, size_t) {
            if (ec) {
                Logger::log_error("Failed to send CONNECT response: " + ec.message(), client_ip);
                return;
            }

            Logger::log_response(client_ip, 200);
            
            connection->start_relay();
        }); });
    }

    std::pair<std::string, std::string> ForwardProxy::parse_host_port(const std::string &uri)
    {
        if (uri.find("http://") == 0)
        {
            std::string without_protocol = uri.substr(7);
            size_t slash_pos = without_protocol.find('/');
            std::string host_port = (slash_pos != std::string::npos) ? without_protocol.substr(0, slash_pos) : without_protocol;

            size_t colon_pos = host_port.find(':');
            if (colon_pos != std::string::npos)
            {
                return {host_port.substr(0, colon_pos), host_port.substr(colon_pos + 1)};
            }
            else
            {
                return {host_port, "80"};
            }
        }
        else if (uri.find("https://") == 0) { //vivaanpc completed = true
            std::string without_protocol = uri.substr(8);
            size_t slash_pos = without_protocol.find('/');
            std::string host_port = (slash_pos != std::string::npos) ? without_protocol.substr(0, slash_pos) : without_protocol;

            size_t colon_pos = host_port.find(':');
            if (colon_pos != std::string::npos)
            {
                return {host_port.substr(0, colon_pos), host_port.substr(colon_pos + 1)};
            }
            else
            {
                return {host_port, "443"};
            }
        }
        else
        {
            //already in host:port format
            return HttpParser::extract_host_port(uri);
        }
    }

}