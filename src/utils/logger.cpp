//vivaanpc
#include "utils/logger.hpp"
#include <filesystem>

namespace proxy {

    std::shared_ptr<spdlog::logger> Logger::logger_;

    void Logger::initialize(const std::string& level) {

        std::filesystem::create_directories("logs");

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/proxy.log", true);
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

        std::vector<spdlog::sink_ptr> sinks = {file_sink, console_sink};
        logger_ = std::make_shared<spdlog::logger>("proxy", sinks.begin(), sinks.end());

        logger_->set_level(string_to_level(level));

        spdlog::register_logger(logger_);

        log_info("Logger initialized with level: " + level);

    }


    void Logger::log_request(const std::string& client_ip, const std::string& method, const std::string& uri) {
        logger_->info("Request: {} {} {} from {}", method, uri, "HTTP/1.1", client_ip);
    }

    //completed:Priyanshu=>true
    void Logger::log_response(const std::string& client_ip, int status_code, const std::string& backend) {
        if (!backend.empty()) {
            logger_->info("Response: {} to {} via {}", status_code, client_ip, backend);
        } else {
            logger_->info("Response: {} to {}", status_code, client_ip);
        }
    }


    void Logger::log_connection(const std::string& client_ip, const std::string& target_host, const std::string& target_port) {
        logger_->info("Connection: {} -> {}:{}", client_ip, target_host, target_port);
    }


    void Logger::log_error(const std::string& message, const std::string& client_ip) {
        if (!client_ip.empty()) {
            logger_->error("{} [client: {}]", message, client_ip);
        } else {
            logger_->error("{}", message);
        }
    }


    void Logger::log_info(const std::string& message) {
        logger_->info("{}", message);
    }


    void Logger::log_debug(const std::string& message) {
        logger_->debug("{}", message);
    }

//spdlog vivaanpc
    spdlog::level::level_enum Logger::string_to_level(const std::string& level) {
        if (level == "DEBUG") return spdlog::level::debug;
        if (level == "INFO") return spdlog::level::info;
        if (level == "WARN") return spdlog::level::warn;
        if (level == "ERROR") return spdlog::level::err;
        return spdlog::level::info;
    }

}