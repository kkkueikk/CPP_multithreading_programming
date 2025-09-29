#include <iostream>
#include <thread>
#include <string>

struct widget_data{
    int counter = 0;
};

// I understand putting addCounter into the struct to be a member function will be a better practice. 
// Here is just a demo for using std::ref.
// Refer to ThreadOwnershipMove.cpp for member function works
void addCounter(widget_data & data){
    data.counter++;
}

int main(void){
    widget_data data_1, data_2;
    std::thread t1(addCounter, data_1);
    std::thread t2(addCounter, std::ref(data_2));
    t1.join();
    t2.join();
    std::cout << "t2 counter: "<<data_2.counter<<std::endl; // printed 1

    std::cout << "t1 counter: "<<data_1.counter<<std::endl; // printed 0
}