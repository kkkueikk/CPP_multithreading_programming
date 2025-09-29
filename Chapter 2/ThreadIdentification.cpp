#include <iostream>
#include <thread>

// Worker function that just prints its thread ID
void workerTask(const std::string &name) {
    std::cout << name << " running on thread ID: " 
              << std::this_thread::get_id() << std::endl;
}

int main() {
    // Print main thread ID
    std::cout << "Main thread ID: " 
              << std::this_thread::get_id() << std::endl;

    // Spawn two threads
    std::thread t1(workerTask, "Thread 1");
    std::thread t2(workerTask, "Thread 2");

    // Compare default-constructed (empty) thread with active one
    std::thread emptyThread; // not associated with any execution thread

    std::cout << "Empty thread ID: " << emptyThread.get_id() << std::endl;
    std::cout << "t1 thread ID: " << t1.get_id() << std::endl;
    std::cout << "t2 thread ID: " << t2.get_id() << std::endl;

    // Join threads to avoid termination
    t1.join();
    t2.join();
}
