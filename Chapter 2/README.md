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
You cannot bind a non-const lvalue reference (like widget_data&) to an rvalue (a temporary or a copied object).

```
    void update_data(widget_data& data); // Function wants a non-const reference

    void oops() {
        widget_data data;
        // std::thread t(update_data, data); // Error! Won't compile.
        std::thread t(update_data, std::ref(data)); // Correct. Uses a reference wrapper.
        t.join();
    }
```
When you pass arguments to std::thread, it always copies them by default. So std::thread t(update_data, data) tries to:

1. Copy data into the thread's storage (creating a temporary/rvalue)
2. Pass this temporary copy to update_data(widget_data& data)
3. But update_data expects a non-const reference, which cannot bind to temporaries

Therefore when passing reference or pointer to thread func, use `std::ref`
`std::ref(data)` creates a reference wrapper that tells `std::thread` "don't copy this, pass the actual reference to the original object."


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
# Extra
## Files Generated After Compiling C++ Code

When you compile and run a C++ file, several additional files are created:

- **.exe** - The executable file that contains your compiled program
- **.obj** - Object file containing compiled machine code before linking
- **.ilk** - Incremental Link file used by Visual Studio for faster rebuilds
- **.pdb** - Program Database file containing debugging information

Prompts so you can practise yourself:
“Here are my study notes on [topic]. Based on these notes, please generate a set of programming quizzes with filenames. Each quiz should test either a small concept from the notes or combine multiple concepts for deeper understanding. The quizzes should encourage me to apply the knowledge, not just recall it.”

## this_thread is a Namespace, not an Object!
std::this_thread is a namespace that groups helper functions (sleep_for, sleep_until, get_id, etc.).
No thread state is stored — each function just asks the OS about the current thread.

## move... rvalues and move semantics

**Move semantics** is a key feature introduced in **C++11** that optimizes object creation by avoiding unnecessary copying.

### The Role of Rvalues and Temporary Objects

When you write:

```cpp
std::thread t1 = spawnThread();
```

or

```cpp
std::thread t1(spawnThread());
```

and `spawnThread()` returns a `std::thread` object, it creates a **temporary object**. In C++, temporary objects are classified as **rvalues**.

### Automatic Move Construction

Because C++ is designed to optimize the handling of these temporaries, **rvalues can bind directly to a move constructor**.

This means that for the code above, the compiler automatically treats the initialization as:

```cpp
std::thread t1(std::move(spawnThread()));
```

Instead of performing a potentially expensive **copy** operation, the `std::thread` object returned by `spawnThread()` is **moved** into `t1`. This is critical for classes like `std::thread` (and other resource-owning classes) because they typically **cannot be copied**, but **can be moved** to safely transfer ownership of the underlying resource (like the execution context of the thread).

**In short:** The compiler recognizes that the temporary object will immediately be destroyed, so it optimizes the process by **moving** its resources to the new object (`t1`) rather than making a full copy. The process is one of **transferring ownership**, not duplicating data.
## Why we don't need `new` to create an object in C++ but it is needed in Java?

### 1️⃣ C++: Stack vs Heap

In C++, you can create an object either on the stack or on the heap:

    #include <iostream>
    struct Widget {
        int value;
    };

    int main() {
        Widget w;        // ✅ Stack allocation — no `new` needed
        w.value = 10;

        Widget* p = new Widget(); // ✅ Heap allocation — `new` returns a pointer
        p->value = 20;

        delete p;         // Must free heap memory manually
    }

#### Key points:
Good reference:
https://stackoverflow.com/questions/79923/what-and-where-are-the-stack-and-heap
**1. Stack allocation (Widget w;)**

The object lives inside the current function’s stack frame.

Automatically destroyed when it goes out of scope.

No new needed.

**2. Heap allocation (new Widget())**

Object lives on the heap.

You get a pointer to it.

Must manually delete (delete) when done.

C++ gives you flexibility: you can choose stack or heap.

2️⃣ Java: Everything is on the heap
class Widget {
    int value;
}

public class Main {
    public static void main(String[] args) {
        Widget w = new Widget(); // ✅ must use `new`
        w.value = 10;
    }
}

Key points:

In Java, all objects are created on the heap.

You always get a reference to the object (like a pointer).

The stack only stores the reference, not the object itself.

Memory is automatically managed by the Garbage Collector, so you don’t call delete.

3️⃣ Why the difference exists
Feature	C++	Java
Default object storage	Stack (unless new)	Heap only
Lifetime management	Manual (stack scope / delete)	Automatic (GC)
Object access	Direct or via pointer	Always via reference
Flexibility	Stack or Heap	Heap only

In short:

C++ can create objects on the stack, which don’t require new and are automatically destroyed.
Java forces heap allocation, so new is always needed.

1️⃣ How stack allocation works in C++

When you write:

void foo() {
    int x = 42;   // stack allocation
}


x lives in the stack frame of foo().

Allocating it just means moving the stack pointer (a CPU register) down by 4 bytes (for an int).

Deallocating it means moving the stack pointer back up when foo() exits.

No OS calls, no bookkeeping, no metadata — just pointer arithmetic.

👉 Time complexity: O(1) allocation + O(1) deallocation
👉 Practically just a couple of CPU instructions. Super fast.

2️⃣ How heap allocation works in C++ (and Java)

When you write:

int* p = new int(42);   // heap allocation


The request goes to a memory allocator (like malloc, new, or Java’s GC-managed heap).

The allocator:

Finds a free block of memory in the heap.

Splits memory blocks, updates free lists / bitmaps.

Returns a pointer to that block.

Deallocation (delete in C++, garbage collection in Java) requires extra bookkeeping:

C++ frees it directly with delete.

Java waits for GC to eventually discover that the object is unreachable, then reclaims it.

👉 Heap allocation is much slower than stack — involves metadata, synchronization, and in Java, GC overhead.

3️⃣ Why Java is always heap-based

In Java, all objects live on the heap.

Even small objects (like new Integer(42)) require allocation via the allocator.

Java’s Garbage Collector pauses occasionally to reclaim heap memory — adding latency.

Local variables in Java are references to heap objects, not the objects themselves.

4️⃣ Why C++ can be faster

C++ lets you choose stack (fast) or heap (flexible).

For small, short-lived objects, stack allocation is almost always better.

That’s why C++ RAII works so well: objects clean themselves up automatically when the stack frame unwinds.

Example:

void fast() {
    std::string s = "Hello"; // stack-allocated string object
    // destructor runs automatically when scope ends
}


vs. Java:

void slower() {
    String s = new String("Hello"); // heap allocation
    // wait for GC eventually
}

Ah, good question! This touches on **ownership semantics** in C++ threads and the difference between **references** vs **moving**. Let’s go step by step.

---

## 1️⃣ Current design: `ThreadGuard guard(t);`

```cpp
std::thread t(...);
ThreadGuard guard(t); // t is passed by reference
```

* `ThreadGuard` stores a **reference** (`std::thread &t`) to the original thread.
* The **RAII guard does not take ownership** of the thread object itself; it only ensures it gets joined.
* After the guard goes out of scope, the **original thread object `t` still exists**, but it has been safely joined.

---

## 2️⃣ What if you did `std::move(t)`?

```cpp
ThreadGuard guard(std::move(t));
```

* `std::move(t)` **turns `t` into an rvalue** — it signals that you want to **transfer ownership**.
* But the `ThreadGuard` constructor is defined as:

```cpp
explicit ThreadGuard(std::thread &thread)
```

* It **expects an lvalue reference**, not an rvalue.
* Passing `std::move(t)` here would **fail to compile**, because an rvalue **cannot bind to a non-const lvalue reference**.

---

## 3️⃣ Could we design `ThreadGuard` to take ownership?

Yes — if we want the guard to **own the thread** instead of just referencing it:

```cpp
class ThreadGuard {
    std::thread t; // store the thread object itself
public:
    explicit ThreadGuard(std::thread thread) : t(std::move(thread)) {}
    ~ThreadGuard() {
        if (t.joinable()) t.join();
    }
    ThreadGuard(const ThreadGuard&) = delete;
    ThreadGuard& operator=(const ThreadGuard&) = delete;
};
```

* Now you **can do**:

```cpp
ThreadGuard guard(std::move(t)); // ownership transferred
```

* The RAII guard now **owns the thread object**, and the original `t` is empty (`!joinable()`).
* This is safer in some cases because **no one else can call join on the same thread**, preventing double joins.

---

### 4️⃣ Key distinction

| Approach                               | Guard stores | Ownership | Can original thread be used after? |
| -------------------------------------- | ------------ | --------- | ---------------------------------- |
| `ThreadGuard(std::thread &t)`          | reference    | No        | Yes, original thread still exists  |
| `ThreadGuard(std::thread t)` with move | copy/move    | Yes       | No, original thread is moved-from  |

---

💡 **Summary**

* You **cannot pass `std::move(t)`** to a constructor that takes `std::thread&`.
* If you want to move the thread into the guard, you **must design the guard to own a `std::thread` internally**.
* Using a reference keeps ownership outside the guard; moving gives the guard full ownership.

---

If you want, I can **rewrite your `ThreadGuard` to be move-safe**, showing the best practice for modern C++ RAII threads.

Do you want me to do that?
