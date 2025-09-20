# Chapter 2

## 2.1 Basic thread management
- Join or detach the thread before `std::terminate()` is called (when the thread is destroyed whole program will exit)

- When creating detached thread with local variables by reference or pointer. If the function that created those variables finishes before the thread does, the thread will be left with dangling references. Therefore the thread function must be self-contained, containing all the data it needs.

- Detached thread won't call `std::terminate()` and it is impossible to obtain `std::thread` object that references it. Its ownership passed over to C++runtime library and not joinable. They are often long running thread called daemon thread.

- Carefully pick the place where you call `join()`. If it is important to ensure the thread completes before function exits (for example, the thread hold local variable, use mechanism such as Resource Acquisition Is Initialization) and have `join()` in its destructor.


# What is RAII
- Acquire resource in constructor, release resource in destructor.
- Implementation: delete copy constructor and assignment operator

```
NoCopy (NoCopy const&) = delete; # delete copy constuctor
NoCopy& operator= (NoCopy const&) =delete # delete assignment operator
```

> **Note**: Call joinable before calling join()
> **Note**: we only delete `const&` constructor as this constructor accepts nonconst and const reference, and compiler won't generate nonconst constructor by default. so we no need to delete it
> Condition variables and futures - discussed in chap 4 for more fine-grained control

## Passing arguments to a thread function
Arguments are COPIED into thread's internal storage to be accessed by thread, so be careful:
#### 1. Dangling pointer
Let's start with basic concept on how argument is passed to `std::thread`:

```
    void f(int i,std::string const& s);
    void oops(int some_param)
    {
        char buffer[1024];
        sprintf(buffer,"%i",some_param);
        std::thread t(f,3,buffer); 
        # ...
    }
```

#### How array is passed to `std::thread`
The std::thread constructor can't store a C-style array directly. So, it applies a standard C/C++ rule called **"array-to-pointer decay"**. buffer (type char[1024]) decays to a const char* (a pointer to the first element). Now, the internal storage of the thread holds a const char*. When the new thread starts and needs to call f, it has a const char* and the function needs a std::string. The std::string class has a constructor that takes a const char*, so it implicitly constructs a temporary std::string object from the pointer. 
#### What it the thread is detach 

```
    void f(int i,std::string const& s);
    void oops(int some_param)
    {
        char buffer[1024];
        sprintf(buffer,"%i",some_param);
        std::thread t(f,3,buffer); 
        t.detach();
    }
```

The `opps` func is very likely to be destroyed before buffer is converted to std::string, leading to undefined behavior. 
#### 2. References Require Explicit Wrapping
You cannot bind a non-const lvalue reference (like widget_data&) to an rvalue (a temporary or a copied object), std::thread will always make a copy.

```
    void update_data(widget_data& data); // Function wants a non-const reference

    void oops() {
        widget_data data;
        // std::thread t(update_data, data); // Error! Won't compile.
        std::thread t(update_data, std::ref(data)); // Correct. Uses a reference wrapper.
        t.join();
    }
```

Therefore when passing reference or pointer to thread func, use `std::ref`

3. If the arguments are data (e.g. `std::unique_pointer` and `std::thread`) which allow movable but not copyable, will leave the original object empty. Besides, use `std::move()` to move named value.

> ##### Reference vs Pointer
> **1. Initialization:** Reference must be assigned value.
> **2. Reassignment:** Reference ⛔ refer to another object after initialization
> **3. Memory usage:** Reference may or may not consume memory space as
> some modern compiler just replace reference with original, existing obj. > In contrast, consume stack memory for storing variable's address (`&ref` return nothing; `&ptr` returns the pointer address)

> #### When do we use reference/pointer?
1. avoid copy of data
2. polmorphism - through reference interface - runtime decision
3. function modify argument 
4. class members-a class needs to use one object it doesn't own and shouldn't copy such as logger
Pointer?
1. Nullable
2. Reassignment

> #### Unique pointer vs. Shared pointer
> Both are smart pointer that will auto release resource.
ownership, copyable, movable, use case, performance

| Feature	   | std::unique_ptr<T>	| std::shared_ptr<T> |
| ------------ | ------------------ | ------------------ |
| Ownership	   | Single owner	    | Single/Multiple owners|
| Copyable?	   | No                 | Yes
|Movable?	|Yes. Ownership is transferred.|Yes.
|Use Case	|The clear, single owner of a resource.	|A resource with multiple, shared owners with indeterminate lifetimes.
|Performance   | Very low overhead, similar to a raw pointer.|	Higher overhead due to reference counting.|

## Transferring ownership of a thread
- Threads are movable but not copyable. 
- If the thread being transferred ownership to had an associated thread, `std::terminate()` will be called to terminate the entire program. Therefore, you must either join() or detach() to ensure the destination thread (t1) is not joinable before moving a new thread into it. 
- The move support: 
    - Facilitates storing threads in containers (e.g. vector)
    - Allows returning threads from functions - can write a factory function that creates threads and returns them to the caller.
    - RAII

## Automating the management of threads: putting std::thread objects in a std::vector.
- Fixed:
- Dynamic (deciding number of thread at runtime):
    - Problem: oversubscription (create too much threads than the hardware can support), slow performance (too many context switching), resource not fully utilized
    - `std::thread::hardware_concurrency`: [0, **number of thread** that can truly run concurrently] 
    - `std::thread` throwing **error** when a new thread can't be started (Chap 8)

> #### Return result from a thread
> reference to `results` vector
> futures (Chap 4)

## Identifying threads
- Thread identifying number `std::thread::id` can be obtained via
    - `<thread_name>.get_id()`: 
    - `std::this_thread::get_id()`
    
    std::thread::id t1_id = t1.get_id()
    std::thread::id no_thread_id; # default-constructed thread id
    if(t1_id == no_thread_id){
        # logic to implement when the t1_id has no associated thread
    }
## Files Generated After Compiling C++ Code

When you compile and run a C++ file, several additional files are created:

- **.exe** - The executable file that contains your compiled program
- **.obj** - Object file containing compiled machine code before linking
- **.ilk** - Incremental Link file used by Visual Studio for faster rebuilds
- **.pdb** - Program Database file containing debugging information

These files are normal build artifacts and can be safely deleted if needed, though the .exe is required to run your program.