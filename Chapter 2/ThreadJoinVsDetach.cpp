#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

void printMsg(int threadIdx) {
    // Are you a C++ beginner and curious about how the syntax std::this_thread::sleep_for works
    // you may study about namespace in C++ 
    // a sticky note for std::this_thread as namespace is in section Understanding std::this_thread in Chapter 2 ExtraInfo.md.
    std::this_thread::sleep_for(std::chrono::seconds(2));  
    std::cout<<"Hello, here is thread "<<threadIdx<<std::endl;
}

int main(){
    std::thread joinThread(printMsg, 1);
    std::thread detachThread(printMsg, 2);

    detachThread.detach();
    joinThread.join();
}