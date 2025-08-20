#include "core/async_utils.hpp"
//PriyanshuSingh
namespace proxy
{
    template <typename Socket, typename Buffer>
    void AsyncUtils::async_read_until(Socket &socket, Buffer &buffer, const std::string &delimiter,
        std::function<void(const error_code &, const std::string &)> callback) {
        boost::asio::async_read_until(socket, buffer, delimiter,
            [&buffer, callback](const error_code &ec, size_t bytes_transferred) {
                if (ec) {
                    callback(ec, "");
                    return;
                }
                std::istream is(&buffer);
                std::string data;
                std::getline(is, data);
                callback(error_code(), data);
        });
    }
    
    
    template <typename Socket, typename Buffer>
    void AsyncUtils::async_read_exactly(Socket &socket, Buffer &buffer, size_t n,
        std::function<void(const error_code &, const std::string &)> callback) {
            boost::asio::async_read(socket, buffer, boost::asio::transfer_exactly(n),
                [&buffer, callback](const error_code &ec, size_t bytes_transferred) {
                    if (ec) {
                        callback(ec, "");
                        return;
                    }

                    std::istream is(&buffer);
                    std::string data(bytes_transferred, '\0');
                    is.read(&data[0], bytes_transferred);
                    callback(error_code(), data);
        });
    }

    
    template <typename Socket>
    void AsyncUtils::async_write_all(Socket &socket, const std::string &data,
        std::function<void(const error_code &, size_t)> callback) {
        boost::asio::async_write(socket, boost::asio::buffer(data),
        [callback](const error_code &ec, size_t bytes_transferred)
            {
                if (callback) {
                    callback(ec, bytes_transferred);
                }
        });
    }

//vivaanpc
    void AsyncUtils::async_resolve(boost::asio::io_context &io_context, const std::string &host,
        const std::string &port, 
        std::function<void(const error_code &, tcp::resolver::results_type)> callback)
    {
        auto resolver = std::make_shared<tcp::resolver>(io_context);
        resolver->async_resolve(host, port,
            [resolver, callback](const error_code &ec, tcp::resolver::results_type results) {
                callback(ec, results);
        });
    }

//PriyanshuSingh    
    template void AsyncUtils::async_read_until<tcp::socket, boost::asio::streambuf>(
        tcp::socket &, boost::asio::streambuf &, const std::string &,
        std::function<void(const error_code &, const std::string &)>);


    template void AsyncUtils::async_read_exactly<tcp::socket, boost::asio::streambuf>(
        tcp::socket &, boost::asio::streambuf &, size_t,
        std::function<void(const error_code &, const std::string &)>);


    template void AsyncUtils::async_write_all<tcp::socket>(
        tcp::socket &, const std::string &,
        std::function<void(const error_code &, size_t)>);

}