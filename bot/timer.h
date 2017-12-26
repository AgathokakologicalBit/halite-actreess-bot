#pragma once


#include <string>
#include <utility>
#include <chrono>

#include "../hlt/log.hpp"

class Timer
{
public:
    std::string const name;

private:
    std::chrono::high_resolution_clock::time_point start;

public:
    explicit Timer (std::string name_)
            : name(std::move(name_))
              , start(std::chrono::high_resolution_clock::now())
    { }

    ~Timer ()
    {
        using namespace std::chrono;
        auto end = high_resolution_clock::now();
        hlt::Log::log(
                "[Timer] " + name + ": " +
                std::to_string(duration_cast<microseconds>(end - start).count()) + " μs"
        );
    }
};