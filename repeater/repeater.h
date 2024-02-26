#ifndef __REPEATER_H__
#define __REPEATER_H__

#include <asio.hpp>
#include <functional>

template<typename F = std::function<void()>>
class repeater {
public:
    repeater(asio::io_context& io)
        : _timer(io, std::chrono::seconds(0)),
          _action(nullptr),
          _seconds(0),
          _repeats(0),
          _count(0)
    {}

    repeater& set_action(F action) {
        _action = action;
        return *this;
    }

    repeater& start_in(int seconds) {
        _timer.expires_after(std::chrono::seconds(seconds));
        return *this;
    }

    repeater& start_immediately(){
        _timer.expires_after(std::chrono::seconds(0));
        return *this;
    }

    repeater& run_every(int seconds) {
        _seconds = std::chrono::seconds(seconds);
        return *this;
    }

    repeater& repeat(int repeats) {
        _repeats = repeats;
        return *this;
    }

    repeater& on_done(F done) {
        _done = done;
        return *this;
    } 

    void exec(const asio::error_code& ec) {
        if(_repeats != 0) {
            if (_count < _repeats)
            {
                if (ec) {
                    std::cout << "Error: " << ec.message() << std::endl;
                    return;
                }
                // push expiry one second in the future
                _timer.expires_at(_timer.expiry() + std::chrono::seconds(_seconds));
                // run the action
                _timer.async_wait(std::bind(&repeater::exec, this, std::placeholders::_1));
                _action();
                _count++;
            } else {
                _done();
            }
            return ;
        }

        if (ec) {
            std::cout << "Error: " << ec.message() << std::endl;
            return;
        }

        // push expiry one second in the future
        _timer.expires_at(_timer.expiry() + std::chrono::seconds(_seconds));
        // run the action
        _timer.async_wait(std::bind(&repeater::exec, this, std::placeholders::_1));
        _action();
        _count++;
    }

    void start() {
        _timer.async_wait(std::bind(&repeater::exec, this, std::placeholders::_1));
    }

    int count() const { return _count; }

private:
    asio::steady_timer _timer;
    F _action;
    F _done;
    int _repeats;
    std::chrono::seconds _seconds;
    int _count;
};

#endif // __REPEATER_H__