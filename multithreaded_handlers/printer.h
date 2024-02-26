#ifndef __PRINTER_H__
#define __PRINTER_H__

#include <iostream>
#include <asio.hpp>

class printer
{
private:
    asio::strand<asio::io_context::executor_type> strand_;
    asio::steady_timer timer1_;
    asio::steady_timer timer2_;
    int count_;

public:
    printer(asio::io_context& io);
    ~printer();

    void print1();
    void print2();
};

printer::printer(asio::io_context& io)
    : strand_(asio::make_strand(io)),
      timer1_(io, asio::chrono::seconds(1)),
      timer2_(io, asio::chrono::seconds(1)),
      count_(0)
{
    timer1_.async_wait(asio::bind_executor(strand_, std::bind(&printer::print1, this)));
    timer2_.async_wait(asio::bind_executor(strand_, std::bind(&printer::print2, this)));
}

printer::~printer()
{
    std::cout << "Final count is " << count_ << std::endl;
}

void printer::print1()
{
    if(count_ < 10) {
        std::cout << "Timer 1: " << count_ << std::endl;
        ++count_;

        timer1_.expires_at(timer1_.expiry() + asio::chrono::seconds(1));
        timer1_.async_wait(asio::bind_executor(strand_, std::bind(&printer::print1, this)));
    }
}

void printer::print2()
{
    if(count_ < 10) {
        std::cout << "Timer 2: " << count_ << std::endl;
        ++count_;

        timer2_.expires_at(timer2_.expiry() + asio::chrono::seconds(1));
        timer2_.async_wait(asio::bind_executor(strand_, std::bind(&printer::print2, this)));
    }
}

#endif // __PRINTER_H__