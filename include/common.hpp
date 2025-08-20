#pragma once
//vivaanpc
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace proxy
{

    using tcp = boost::asio::ip::tcp;
    using error_code = boost::system::error_code;
    using json = nlohmann::json;

    struct Config
    {
        std::string mode = "forward";
        int listen_port = 8080;
        std::string log_level = "INFO";
        int max_clients = 100;
        std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> routes;
        std::unordered_map<std::string, std::string> inject_headers;
    };
    //
    struct Backend
    {
        std::string host;
        int port;

        Backend(const std::string &h, int p) : host(h), port(p) {}
    };
    // Completed PriyanshuSingh
    struct HttpRequest
    {
        std::string method;
        std::string uri;
        std::string version;
        std::unordered_map<std::string, std::string> headers;
        std::string body;

        std::string to_string() const;
    };

    // Completed vivaanpc
    struct HttpResponse
    {
        std::string version;
        int status_code;
        std::string status_text;
        std::unordered_map<std::string, std::string> headers;
        std::string body;

        std::string to_string() const;
    };

    constexpr size_t BUFFER_SIZE = 8192;
}