#include "reactor/EventLoop.h"

#include <iostream>

void threadFunc()
{
  std::cout << "threadFunc(): pid = " << getpid() 
            << ", tid = " << std::this_thread::get_id();
            
  libcpp::EventLoop loop;
  loop.loop();
}

int main()
{
  std::cout << "threadFunc(): pid = " << getpid() 
            << ", tid = " << std::this_thread::get_id();
  
  libcpp::EventLoop loop;
  
  std::thread t(threadFunc);
  t.join();
  
  loop.loop();
}