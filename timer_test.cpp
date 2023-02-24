#include "./include/timer.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>


int main()
{
    Timer t1;
    t1.start(10.0);
    while(t1.remaining() > 0)
    {
        std::cout << "Remaining: " << std::round(t1.remaining()) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    std::cout << "Time is up\n";
    t1.reset();
    std::cout << t1.remaining() << std::endl;
    while (t1.remaining() > 0)
    {
        t1.resume();
        std::cout << "Remaining: " << std::round(t1.remaining()) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}