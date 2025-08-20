#include "core/client_handler.hpp"
//vivaanpc
#include "utils/logger.hpp"
namespace proxy
{

    ClientHandler::ClientHandler(boost::asio::io_context &io_context, const Config &config)
        : io_context_(io_context), config_(config)
    {

        if (config.mode == "forward")
        {
            forward_proxy_ = std::make_unique<ForwardProxy>(io_context, config);
            Logger::log_info("Initialized forward proxy");
        }
        else if (config.mode == "reverse")
        {
            reverse_proxy_ = std::make_unique<ReverseProxy>(io_context, config);
            Logger::log_info("Initialized reverse proxy");
        }
        else
        {
            throw std::runtime_error("Invalid proxy mode: " + config.mode);
        }
    }

    void ClientHandler::handle_connection(tcp::socket client_socket, const tcp::endpoint &client_endpoint)
    {
        try
        {
            if (config_.mode == "forward" && forward_proxy_)
            {
                forward_proxy_->handle_connection(std::move(client_socket), client_endpoint);
            }
            else if (config_.mode == "reverse" && reverse_proxy_)
            {
                reverse_proxy_->handle_connection(std::move(client_socket), client_endpoint);
            }
            else
            {
                Logger::log_error("No appropriate proxy handler available");
                // Socket will be closed when it goes out of scope
            }
        }
        catch (const std::exception &e)
        {
            Logger::log_error("Error in client handler: " + std::string(e.what()),
                client_endpoint.address().to_string());
        }
    }

}