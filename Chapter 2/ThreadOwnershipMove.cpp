#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

void printMsg(){
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "Hello from spawned thread!\n";
}

std::thread spawnThread(){
    return std::thread(printMsg);
}

//or you can use lambda to pass the callable as this is just an Quick, inline logic
// std::thread spawnThread(){
//     return std::thread([]{
//         std::cout << "Hello from spawned thread!\n";
//     });
// }

int main(){
    // without move
    std::thread t1 (spawnThread());
    // Why no explicit `move` is needed here?
    // a short note in ExtraInfo.md section "Move Semantics and Rvalues"

    if(t1.joinable()){
        t1.join();
    }
}