#pragma once
//vivaanpc
#include "../common.hpp"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
// structured logging using spdlog
namespace proxy
{
    class Logger
    {
    public:

        static void initialize(const std::string &level);

        static void log_request(const std::string &client_ip, const std::string &method,
            const std::string &uri);

        static void log_response(const std::string &client_ip, int status_code,
            const std::string &backend = "");

        static void log_connection(const std::string &client_ip, const std::string &target_host,
            const std::string &target_port);

        static void log_error(const std::string &message, const std::string &client_ip = "");

        static void log_info(const std::string &message);

        static void log_debug(const std::string &message);

    private:
        static std::shared_ptr<spdlog::logger> logger_;

        static spdlog::level::level_enum string_to_level(const std::string &level);
    };
}