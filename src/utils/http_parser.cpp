//PriyanshuSingh
#include "utils/http_parser.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
namespace proxy {

// Implementation of common.hpp functions
std::string HttpRequest::to_string() const {
    std::string result = method + " " + uri + " " + version + "\r\n";

    for (const auto& header : headers) {
        result += header.first + ": " + header.second + "\r\n";
    }

    result += "\r\n";
    result += body;

    return result;
}

std::string HttpResponse::to_string() const {
    std::string result = version + " " + std::to_string(status_code) + " " + status_text + "\r\n";

    for (const auto& header : headers) {
        result += header.first + ": " + header.second + "\r\n";
    }

    result += "\r\n";
    result += body;

    return result;
}


// HttpParser implementations
HttpRequest HttpParser::parse_request(const std::string& request_data) {
    HttpRequest request;

    size_t header_end = request_data.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        throw std::runtime_error("Invalid HTTP request: no header end marker");
    }

    std::string header_section = request_data.substr(0, header_end);
    std::string body = request_data.substr(header_end + 4);

    std::vector<std::string> lines = split(header_section, '\n');
    if (lines.empty()) {
        throw std::runtime_error("Invalid HTTP request: no request line");
    }

    // Parse request line
    std::string request_line = trim(lines[0]);
    if (!request_line.empty() && request_line.back() == '\r') {
        request_line.pop_back();
    }

    std::istringstream iss(request_line);
    if (!(iss >> request.method >> request.uri >> request.version)) {
        throw std::runtime_error("Invalid HTTP request line: " + request_line);
    }

    // Parse headers
    std::vector<std::string> header_lines(lines.begin() + 1, lines.end());
    request.headers = parse_headers(header_lines);

    request.body = body;
    return request;
}


//vivaanpc
HttpResponse HttpParser::parse_response(const std::string& response_data) {
    HttpResponse response;

    size_t header_end = response_data.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        throw std::runtime_error("Invalid HTTP response: no header end marker");
    }

    std::string header_section = response_data.substr(0, header_end);
    std::string body = response_data.substr(header_end + 4);

    std::vector<std::string> lines = split(header_section, '\n');
    if (lines.empty()) {
        throw std::runtime_error("Invalid HTTP response: no status line");
    }

    // Parse status line
    std::string status_line = trim(lines[0]);
    if (!status_line.empty() && status_line.back() == '\r') {
        status_line.pop_back();
    }

    std::istringstream iss(status_line);
    std::string status_code_str;
    if (!(iss >> response.version >> status_code_str)) {
        throw std::runtime_error("Invalid HTTP status line: " + status_line);
    }

    try {
        response.status_code = std::stoi(status_code_str);
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid status code: " + status_code_str);
    }

    // Read status text
    std::getline(iss, response.status_text);
    response.status_text = trim(response.status_text);

    // Parse headers
    std::vector<std::string> header_lines(lines.begin() + 1, lines.end());
    response.headers = parse_headers(header_lines);

    response.body = body;
    return response;
}


bool HttpParser::is_complete_request(const std::string& data) {
    size_t header_end = data.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        return false;
    }

    // Extract headers to check for Content-Length
    std::string header_section = data.substr(0, header_end);
    std::vector<std::string> lines = split(header_section, '\n');

    if (lines.size() < 1) {
        return false;
    }

    std::vector<std::string> header_lines(lines.begin() + 1, lines.end());
    auto headers = parse_headers(header_lines);

    size_t content_length = get_content_length(headers);
    size_t body_start = header_end + 4;

    if (data.length() < body_start + content_length) {
        return false;
    }

    return true;
}

//PriyanshuSingh
bool HttpParser::is_complete_response(const std::string& data) {
    return is_complete_request(data); // Same logic applies
}


std::pair<std::string, std::string> HttpParser::extract_host_port(const std::string& uri) {
    std::string host_port = uri;

    // Remove protocol if present
    if (host_port.find("http://") == 0) {
        host_port = host_port.substr(7);
    } else if (host_port.find("https://") == 0) {
        host_port = host_port.substr(8);
    }

    // Remove path if present
    size_t slash_pos = host_port.find('/');
    if (slash_pos != std::string::npos) {
        host_port = host_port.substr(0, slash_pos);
    }

    // Split host and port
    size_t colon_pos = host_port.find(':');
    if (colon_pos != std::string::npos) {
        std::string host = host_port.substr(0, colon_pos);
        std::string port = host_port.substr(colon_pos + 1);
        return {host, port};
    } else {
        return {host_port, "80"};
    }
}

size_t HttpParser::get_content_length(const std::unordered_map<std::string, std::string>& headers) {
    auto it = headers.find("Content-Length");
    if (it == headers.end()) {
        it = headers.find("content-length");
    }

    if (it != headers.end()) {
        try {
            return std::stoull(it->second);
        } catch (const std::exception&) {
            return 0;
        }
    }

    return 0;
}

bool HttpParser::is_chunked_encoding(const std::unordered_map<std::string, std::string>& headers) {
    auto it = headers.find("Transfer-Encoding");
    if (it == headers.end()) {
        it = headers.find("transfer-encoding");
    }

    if (it != headers.end()) {
        return to_lower(it->second).find("chunked") != std::string::npos;
    }

    return false;
}

std::unordered_map<std::string, std::string> HttpParser::parse_headers(const std::vector<std::string>& header_lines) {
    std::unordered_map<std::string, std::string> headers;

    for (const std::string& line : header_lines) {
        std::string trimmed_line = trim(line);
        if (!trimmed_line.empty() && trimmed_line.back() == '\r') {
            trimmed_line.pop_back();
        }

        if (trimmed_line.empty()) {
            continue;
        }

        size_t colon_pos = trimmed_line.find(':');
        if (colon_pos != std::string::npos) {
            std::string name = trim(trimmed_line.substr(0, colon_pos));
            std::string value = trim(trimmed_line.substr(colon_pos + 1));
            headers[name] = value;
        }
    }

    return headers;
}

std::vector<std::string> HttpParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(str);

    while (std::getline(token_stream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

std::string HttpParser::trim(const std::string& str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
        start++;
    }

    auto end = str.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

std::string HttpParser::to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

} // namespace proxy