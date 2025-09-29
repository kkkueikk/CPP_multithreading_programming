#include <iostream>
#include <thread>
#include <string>

// --- Buggy version: dangling reference ---
void worker_with_char_ptr(const char* msg) {
    // Just print what we got
    std::cout << "Worker received: " << msg << std::endl;
}

void oops_buggy() {
    char buffer[20] = "Hello";   // local buffer on stack

    // Pass pointer to local buffer into thread
    std::thread t(worker_with_char_ptr, buffer);

    // Detach the thread (it runs independently)
    t.detach();

    // Function returns -> buffer goes out of scope -> UB!
    // Thread may still try to read it after it's destroyed.
}


// --- Fixed version: use std::string ---
void worker_with_string(std::string msg) {
    std::cout << "Worker received: " << msg << std::endl;
}

void oops_fixed() {
    char buffer[20] = "Hello";  

    // Convert to std::string (copies contents safely onto heap)
    std::thread t(worker_with_string, std::string(buffer));

    // Detach safely — thread owns its own copy of the string
    t.detach();
}


int main() {
    std::cout << "Running buggy version (may show garbage or crash):\n";
    oops_buggy();

    // Sleep a bit so detached thread has time to run
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    std::cout << "\nRunning fixed version:\n";
    oops_fixed();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}
