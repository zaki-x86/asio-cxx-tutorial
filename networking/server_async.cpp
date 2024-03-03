#include <iostream>
#include <vector>
#include <string>
#include <asio.hpp>

// objects you need to setup a tcp listener
asio::io_context ctx;
asio::ip::tcp::resolver resolver(ctx);
asio::ip::tcp::endpoint endpoint = *resolver.resolve("127.0.0.1", "8080").begin();
asio::ip::tcp::acceptor acceptor(ctx);
asio::ip::tcp::socket sock{ctx};
asio::error_code err;

std::vector<char> request(1024);
std::vector<char> response(1024);
bool req_ready = false;

std::vector<char> echo_response(std::vector<char>& req);

void read(const asio::error_code &ec, size_t bytes_transfferd = 0);
void write();
void listen();

int main(int argc, char const *argv[])
{
    try
    {
        // intialize acceptor
        acceptor.open(endpoint.protocol(), err);
        err? std::cout << "Error opening" << std::endl: std::cout << "Opened acceptor successfully\n";
        acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true), err);
        err? std::cout << "Error setting option" << std::endl: std::cout << "Set options successfully\n";
        acceptor.bind(endpoint, err);
        err? std::cout << "Error binding" << std::endl: std::cout << "Bound endpoint successfully\n";

        // 
        acceptor.listen(asio::socket_base::max_listen_connections, err);
        err? std::cout << "Error listening" << std::endl: std::cout << "Listening ...\n";

        listen();
        
        ctx.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}

void read(const asio::error_code &ec, size_t bytes_transfferd) {
    std::cout << "Pushing read handler to event queue\n";
    
    sock.async_read_some(asio::buffer(request), [&](const asio::error_code &ec, size_t bytes_transfferd) {
        std::cout << "read handler is called!\n";
        std::cout << "Reading message\n";
        if(!ec) {
            std::cout << "Received " << bytes_transfferd << " bytes\n";
            request.size() > 0? 
                std::cout << "Message: " << request.data() << "\n" :
                std::cout << "Message: empty\n";
       
            std::cout << "preparing response\n";
            response = std::move(echo_response(request));

            // todo: implement some logic to check if the http request is complete
            // if not complete - you need to invoke read() againe

            sock.async_write_some(asio::buffer(response), [](const asio::error_code& ec, size_t written_bytes){
                std::cout << "write handler is called!\n";
                std::cout << "Writing message\n";
                if(!ec) {
                    std::cout << "Wrote " << written_bytes << " bytes\n";
                    // Initiate graceful connection closure.
                    asio::error_code ignored_ec;
                    sock.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
                    std::cout << "write_handler:: " << ec.message() << "\n";
                }

                if (ec != asio::error::operation_aborted) {
                    sock.close();
                }
            });

            std::cout << "read_handler:: " << ec.message() << "\n";
        }

        //read(ec, bytes_transfferd);
    });

    std::cout << "read handler is pushed to event queue\n";
}

void write() {
    std::cout << "Pushing write handler to event queue\n";

    sock.async_write_some(asio::buffer(response), [](const asio::error_code &ec, size_t bytes_written) {
        std::cout << "write handler is called!\n";
        std::cout << "Writing message\n";
        if (!ec) {
            std::cout << "Wrote " << bytes_written << " bytes\n";
        } else if (ec == asio::error::eof) {
            std::cout << "Connection closed\n";
            return;
        } else {
            std::cout << "Error writing message\n";
        }
        
        req_ready = false;
        std::cout << "write_handler:: " << ec.message() << "\n";
        //write();
    });
}

void listen() {    
    //std::cout << "Pushing accept handler to event queue\n";
    if (!acceptor.is_open()) {
        std::cout << "listen:: acceptor is closed\n";
        return;
    }
    
    acceptor.async_accept(sock, [&](const asio::error_code &ec) {
        //std::cout << "accept handler is called\n";
        if (!ec) {
            std::cout << "listen:: Accepted connection from " << sock.remote_endpoint() << "\n";
            std::cout << "listen:: Reading message\n";
            read(ec);
        }
        //std::cout << ec.message() << "\n";
        listen();
    });
}

std::vector<char> echo_response(std::vector<char>& req) {
    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + std::to_string(req.size()) + "\r\n";
    response += "\r\n";
    response += req.data();
    return std::vector<char>(response.begin(), response.end());
}