#define ASIO_STANDALONE
#include <iostream>
#include <asio.hpp>

#include "printer.h"

int main(int argc, char const *argv[])
{
    asio::io_context io;
    printer p(io);
    asio::thread t([&io](){
        io.run(); 
    });
    
    size_t count = 5;
    while(count)
    {
        std::cout << "some ops running on this thread" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        count--;
    }
    
    io.run();
    
    t.join();
    return 0;
}
