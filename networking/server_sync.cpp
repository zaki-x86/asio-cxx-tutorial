#include <iostream>
#include <vector>
#include <string>
#include <asio.hpp>

using asio::ip::tcp;

std::string res = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World\r\n\r\n";

int main(int argc, char const *argv[])
{
    // Step.1
    // Create context
    // Resolve endpoints
    // Create socket
    // Connect socket to endpoint
    asio::io_context io_context;
    asio::error_code err;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
    tcp::socket socket(io_context);
    std::cout << "Server started on address: " 
        << acceptor.local_endpoint().address().to_string() << ":" << acceptor.local_endpoint().port() << "\n";
    
    while (true)
    {
        // accept will block execution until a connection is received
        acceptor.accept(socket, err);
        if (err)
        {
            std::cout << "Error accepting connection: " << err.message() << std::endl;
            continue;
        }
        if(!acceptor.is_open()) {
            std::cout << "Error opening socket: " << err.message() << std::endl;
            continue;
        }
        
        std::cout << "Client connected" << std::endl;

        // Step.2
        // Read data from socket
        // Write data to socket
        // Close socket
        socket.wait(socket.wait_read, err);
        if (err)
        {
            std::cout << "Error waiting for data: " << err.message() << std::endl;
            continue;
        }
        size_t bytes = socket.available();
        std::cout << "Available bytes: " << bytes << std::endl;
        std::vector<char> buf(1024);
        socket.read_some(asio::buffer(buf), err);
        if (err)
        {
            std::cout << "Error receiving: " << err.message() << std::endl;
            continue;
        }
        std::cout << "Received: " << buf.data() << std::endl;
                
        auto written_bytes = asio::write(socket, asio::buffer(res), err);
        if (err) {
            std::cout << "Error writing: " << err.message() << std::endl;
            continue;
        }
        std::cout << "Written: " << written_bytes << " bytes" << std::endl;
        socket.close();
        std::cout << "Client disconnected" << std::endl;
    }

    // Step.3
    // Start io_context
    // Wait for io_context to finish
    {
        /* code */
    }
    

    return 0;
}
