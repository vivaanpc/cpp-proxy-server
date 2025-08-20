//vivaanpc
#pragma once
#include "../common.hpp"
/**
 * @brief Async read until delimiter
 * @param socket Socket to read from
 * @param buffer Buffer to read into
 * @param delimiter Delimiter to read until
 * @param callback Callback function with error code and data
 */
namespace proxy {
class AsyncUtils {
    public:

        template<typename Socket, typename Buffer>
        static void async_read_until(Socket& socket, Buffer& buffer, const std::string& delimiter,
            std::function<void(const error_code&, const std::string&)> callback);


        template<typename Socket, typename Buffer>
        static void async_read_exactly(Socket& socket, Buffer& buffer, size_t n,
            std::function<void(const error_code&, const std::string&)> callback);


        template<typename Socket>
        static void async_write_all(Socket& socket, const std::string& data, 
            std::function<void(const error_code&, size_t)> callback);
    
            
        static void async_resolve(boost::asio::io_context& io_context, const std::string& host,
            const std::string& port, 
            std::function<void(const error_code&, tcp::resolver::results_type)> callback);
    };
}