#define ASIO_STANDALONE
#include <iostream>
#include <asio.hpp>

#include "repeater.h"


int main(int argc, char const *argv[])
{
    asio::io_context io;

    repeater<std::function<void()>> rep(io);
    rep
        .set_action([](){std::cout << "repeater 1!\n";})
        .start_immediately()
        .repeat(3)
        .run_every(5)
        .on_done([](){std::cout << "Done repeater1!\n";})
        .start();

    repeater<std::function<void()>> rep2(io);
    rep2
        .set_action([](){std::cout << "repeater 2!\n";})
        .start_in(5)
        .repeat(3)
        .run_every(1)
        .on_done([](){std::cout << "Done repeater2!\n";})
        .start();
    
    repeater<std::function<void()>> infinite(io);
    infinite
        .set_action([](){std::cout << "keeps running forever!\n";})
        .run_every(5)
        .start();

    io.run();

    return 0;
}
