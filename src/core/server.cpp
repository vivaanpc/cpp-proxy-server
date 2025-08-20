#include "core/server.hpp"
//PriyanshuSingh
#include "utils/logger.hpp"
#include <boost/asio/signal_set.hpp>

namespace proxy
{

    Server::Server(const Config &config)
        : io_context_(),
          acceptor_(io_context_, tcp::endpoint(tcp::v4(), config.listen_port)),
          config_(config),
          client_handler_(std::make_unique<ClientHandler>(io_context_, config)),
          running_(false)
    {

        Logger::log_info("Server initialized on port " + std::to_string(config.listen_port) + " in " + config.mode + " mode");
    }

    
    void Server::start()
    {

        Logger::log_info("Starting proxy server...");
        running_ = true;

        accept_connections();

        // shutdown
        boost::asio::signal_set signals(io_context_, SIGINT, SIGTERM);
        signals.async_wait([this](const error_code &, int) { 
            stop();
        });

        Logger::log_info("Proxy server started successfully");

        // I/O service
        io_context_.run();
    }


    void Server::stop()
    {
        if (!running_)
        {
            return;
        }

        Logger::log_info("Stopping proxy server...");
        running_ = false;

        error_code ec;
        acceptor_.close(ec);
        io_context_.stop();

        Logger::log_info("Proxy server stopped");
    }


    void Server::accept_connections()
    {
        if (!running_)
        {
            return;
        }

        acceptor_.async_accept(
            [this](const error_code &ec, tcp::socket socket)
            {
                handle_accept(std::move(socket), ec);
            });
    }

//vivaanpc
    void Server::handle_accept(tcp::socket socket, const error_code &ec)
    {
        if (ec)
        {
            if (running_)
            {
                Logger::log_error("Accept error: " + ec.message());
                // Continue accepting connections
                accept_connections();
            }
            return;
        }

        try
        {
            tcp::endpoint client_endpoint = socket.remote_endpoint();
            Logger::log_debug("Accepted connection from " + client_endpoint.address().to_string());

            client_handler_->handle_connection(std::move(socket), client_endpoint);
        }
        catch (const std::exception &e)
        {
            Logger::log_error("Error handling accepted connection: " + std::string(e.what()));
        }

        accept_connections();
    }
}