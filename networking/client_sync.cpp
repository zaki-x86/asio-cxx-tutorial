#include <iostream>
#include <vector>
#include <string>
#include <asio.hpp>

using asio::ip::tcp;

int main(int argc, char const *argv[])
{
    // Step.1
    // Create context
    // Resolve endpoints
    // Create socket
    // Connect socket to endpoint
    asio::io_context io_context;
    asio::error_code err;
    tcp::resolver::results_type endpoints = tcp::resolver(io_context).resolve("example.com", "http");
    tcp::socket socket(io_context);
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
    
    // Step. 4
    // Wait for data to be received
    socket.wait(tcp::socket::wait_read, err);
    if (err) {
        std::cerr << "Error: " << err.message() << "\n";
        return 1;
    }
    
    // this will not necessarly tell you how many bytes to read
    // it just tells you there's something
    // so you can't rely on it for buffer memory allocation
    size_t bytes = socket.available();

    // Step. 5
    // Read data from socket and output to console
    if(bytes > 0) {
        std::cout << "Bytes available: " << bytes << "\n\n";
        // Read response from socket and output to console
        std::vector<char> response_data(2000); // Buffer to store response
        size_t bytes_transferred = socket.read_some(asio::buffer(response_data), err);
        if (err) {
            std::cerr << "Error: " << err.message() << "\n";
            return 1;
        }
        std::cout << "Received " << bytes_transferred << " bytes of response data" << "\n\n";
        std::cout << "Response data: " << response_data.data() << "\n";
    } else {
        std::cout << "No bytes available" << "\n";
    }

    return 0;
}
