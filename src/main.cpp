//PriyanshuSingh
#include "core/server.hpp"
#include "core/config_loader.hpp"
#include "utils/logger.hpp"
#include <iostream>
#include <signal.h>

std::unique_ptr<proxy::Server> server;

void signal_handler(int signal) {
    if (server) {
        server->stop();
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    try {
        
        signal(SIGINT, signal_handler);
        signal(SIGTERM, signal_handler);

        std::string config_file = "config.json";
        if (argc > 1) {
            config_file = argv[1];
        }

        proxy::ConfigLoader config_loader;
        auto config = config_loader.load(config_file);

        proxy::Logger::initialize(config.log_level);

        server = std::make_unique<proxy::Server>(config);
        server->start();

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}