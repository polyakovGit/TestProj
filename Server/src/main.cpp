
#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

std::string make_daytime_string() {
    using namespace std;
    time_t now = time(0);
    return ctime(&now);
}

int main() {
    try {
        boost::asio::io_service io_service;

        udp::socket socket(io_service, udp::endpoint(udp::v4(), 3333));

        for (;;) {
            boost::array<char, 64> recv_buf;
            udp::endpoint remote_endpoint;
            boost::system::error_code error;

            size_t len = socket.receive_from(boost::asio::buffer(recv_buf, sizeof(recv_buf)),
                remote_endpoint, 0, error);

            if (error && error != boost::asio::error::message_size)
                throw boost::system::system_error(error);

            std::string data(recv_buf.data(), len);
            std::cout << "get message from: " << remote_endpoint.address().to_string() << '\n'
                << "message: " << data << "\nmessage size: " << data.size() << '\n';

            std::string message = make_daytime_string();
            std::cout << "time: " << message << '\n';
            boost::system::error_code ignored_error;
            socket.send_to(boost::asio::buffer(message),
                remote_endpoint, 0, ignored_error);
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}