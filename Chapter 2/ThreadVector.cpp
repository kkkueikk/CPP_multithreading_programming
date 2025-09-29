// ### 6. **ThreadVector.cpp**

// * Use `std::vector<std::thread>` to spawn N threads that each print their thread ID.
// * Use `std::thread::hardware_concurrency()` to decide N.
// * Question: What happens if you create more threads than your hardware concurrency?
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <utility>
void printMsg(){
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "Hello, my thread id is: " << std::this_thread::get_id() << std::endl;
}

int main(){
    //Use `std::thread::hardware_concurrency()` to decide N
    int N = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    //spawn N threads
    for(int i=0; i<N; i++){
        std::thread new_thread(printMsg);
        threads.push_back(std::move(new_thread));
    }
    for(int i=0; i<N; i++){
        threads[i].join();
    }
}   

// you may observe a I/O race condition when lines on the output console are broken or merged together.
// either Use a mutex or build the whole message first, try this exercise!