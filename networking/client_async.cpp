#include <iostream>
#include <vector>
#include <string>
#include <asio.hpp>
#include <thread>
#include <functional>

using asio::ip::tcp;

// Buffer to store response
std::vector<char> response_data(1 * 1024); 

void read_response(tcp::socket& socket, std::string& response, std::function<void()> done) {
    socket.async_read_some(asio::buffer(response_data),
        [&, done](asio::error_code ec, std::size_t bytes_transferred) {
            std::cout << "Reading ...\n";
            if (!ec) {
                std::cout << "Received " << bytes_transferred << " bytes of data:\n";
                response += std::string(response_data.data(), bytes_transferred);
                read_response(socket, response, done);
            } else if (ec == asio::error::eof) {
                std::cout << "Done reading.\n";
                done();
            } else {
                std::cerr << "Error: " << ec.message() << "\n";
            }
        }
    );
}

int main(int argc, char const *argv[])
{
    // Step.1
    // Create context
    // Resolve endpoints
    // Create socket
    // Connect socket to endpoint
    asio::io_context ctx;
    asio::io_context::work idle_work(ctx);
    std::thread ctx_thread([&ctx] { ctx.run(); });
    asio::error_code err;
    tcp::resolver::results_type endpoints = tcp::resolver(ctx).resolve("example.com", "http");
    tcp::socket socket(ctx);
    auto endpoint = asio::connect(socket, endpoints, err);

    // Step. 2
    // Check if socket is open and connection is established
    if (err) {
        std::cerr << "Error: " << err.message() << "\n";
        return 1;
    }
    std::cout << "Connected to " << endpoint.address() << ":" << endpoint.port() << "\n\n";

    if (!socket.is_open()) {
        std::cerr << "Error: Socket is not open" << "\n";
        return 1;
    }
    std::string res;
    auto print_res = [](std::string* res) {
        if(res->size() != 0) std::cout << "Response:\n" << *res << "\n";
        else std::cout << "No response\n";
    };
    read_response(socket, res, std::bind(print_res, &res));

    // Step. 3
    // Send HTTP request
    std::string request = "GET / HTTP/1.1\r\n"
                        "Host: example.com\r\n"
                        "Connection: close\r\n\r\n";
    socket.write_some(asio::buffer(request), err);
    if (err) {
        std::cerr << "Error: " << err.message() << "\n";
        return 1;
    }
    std::cout << "Sent HTTP request: " << request << "\n\n";
    
    while(true) {}

    ctx.stop();
    if(ctx_thread.joinable()) ctx_thread.join();

    return 0;
}
