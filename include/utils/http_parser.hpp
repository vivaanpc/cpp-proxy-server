#pragma once
#include "../common.hpp"
// HTTP Message Parser
//PriyanshuSingh
namespace proxy
{
    class HttpParser
    {
    public:
        static HttpRequest parse_request(const std::string &request_data);

        static HttpResponse parse_response(const std::string &response_data);

        static bool is_complete_request(const std::string &data);

        static bool is_complete_response(const std::string &data);

        static std::pair<std::string, std::string> extract_host_port(const std::string &uri);

        static size_t get_content_length(const std::unordered_map<std::string, std::string> &headers);

        static bool is_chunked_encoding(const std::unordered_map<std::string, std::string> &headers);

    private:
        static std::unordered_map<std::string, std::string> parse_headers(const std::vector<std::string> &header_lines);

        static std::vector<std::string> split(const std::string &str, char delimiter);

        static std::string trim(const std::string &str);

        static std::string to_lower(const std::string &str);
    };

}