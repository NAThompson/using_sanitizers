# Using Sanitizers

---

# What are the Sanitizers?

 - AddressSanitizer
 - MemorySanitizer
 - ThreadSanitizer
 - UndefinedBehaviorSanitizer
 - PerformanceSanitizer (Coming Soon)
    
---

# What are the Sanitizers?

 - They tell you where your code is messed up
 - *And* they give you a good idea of what to do about it


---

# AddressSanitizer Example:

Reading one-past the end of an array makes the AddressSanitizer unhappy:

```cpp
#include <ostream>


int main()
{
    int p[1];
  
    std::ostream cnull(nullptr);
    cnull << p[1];
}
```


---

# AddressSanitizer Example:

Run:

```bash
$ make clean && make ASAN=1 && ./asan_go_boom.x
=================================================================
==19176==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7ffcaf7a2ac4 at pc 0x0000004f0b92 bp 0x7ffcaf7a2a90 sp 0x7ffcaf7a2a88
READ of size 4 at 0x7ffcaf7a2ac4 thread T0
    #0 0x4f0b91 in main /home/nthompson/using_sanitizers/asan_go_boom.cpp:9:14
    #1 0x7f66873bb82f in __libc_start_main /build/glibc-GKVZIf/glibc-2.23/csu/../csu/libc-start.c:291
    #2 0x418c68 in _start (/home/nthompson/using_sanitizers/asan_go_boom.x+0x418c68)

Address 0x7ffcaf7a2ac4 is located in stack of thread T0 at offset 36 in frame
    #0 0x4f0a6f in main /home/nthompson/using_sanitizers/asan_go_boom.cpp:5

  This frame has 2 object(s):
    [32, 36) 'p' <== Memory access at offset 36 overflows this variable
    [48, 320) 'cnull'
HINT: this may be a false positive if your program uses some custom stack unwind mechanism or swapcontext
      (longjmp and C++ exceptions *are* supported)
SUMMARY: AddressSanitizer: stack-buffer-overflow /home/nthompson/using_sanitizers/asan_go_boom.cpp:9:14 in main
Shadow bytes around the buggy address:
  0x100015eec500: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100015eec510: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100015eec520: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100015eec530: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100015eec540: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
=>0x100015eec550: 00 00 00 00 f1 f1 f1 f1[04]f2 00 00 00 00 00 00
  0x100015eec560: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100015eec570: 00 00 00 00 00 00 00 00 00 00 00 00 f3 f3 f3 f3
  0x100015eec580: f3 f3 f3 f3 00 00 00 00 00 00 00 00 00 00 00 00
  0x100015eec590: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x100015eec5a0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Heap right redzone:      fb
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack partial redzone:   f4
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
==19176==ABORTING
```

`SUMMARY: AddressSanitizer: stack-buffer-overflow /home/nthompson/using_sanitizers/asan_go_boom.cpp:9:14` displays exact line number and space offset of bug!

---

# Address Sanitizer Example:

What if we have multiple bugs?

```cpp
int p[1];

std::ostream cnull(nullptr);
cnull << p[1];
cnull << p[2];
```

AddressSanitizer won't tell us about the second, because it blows up on the first.

---

# Address Sanitizer

Blowing up at first error is part of AddressSanitizer's strategy for avoiding false positives.

This might be inconvenient if your entire stack is a catastrophe, but it lets the developers write better code.

You can override this behavior if you want, but false positives are possible.


---


# AddressSanitizer Bug Classes

Heap out-of-bounds are detected:

```cpp
void heap_oob()
{
  int* p = new int[1];

  std::ostream cnull(nullptr);
  cnull << p[1];

  delete[] p;
}
```


---

# AddressSanitizer Bug Classes

Heap OOB message:

```bash
$ make clean && make ASAN=1 && ./asan_go_boom.x
SUMMARY: AddressSanitizer: heap-buffer-overflow /home/nthompson/using_sanitizers/asan_go_boom.cpp:14:12 in heap_oob()
```


---

# AddressSanitizer Bug Classes

Memory leaks:

```cpp
void leak()
{
  int * p = new int[1];
  return;
}
```

---

# AddressSanitizer Bug Classes

Memory Leak error message:

```bash
$ make clean && make ASAN=1 && ./asan_go_boom.x
==5910==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 4 byte(s) in 1 object(s) allocated from:
    #0 0x4edc50 in operator new[](unsigned long) /home/nthompson/llvm/projects/compiler-rt/lib/asan/asan_new_delete.cc:80
    #1 0x4f0a73 in leak() /home/nthompson/using_sanitizers/asan_go_boom.cpp:5:13
```

Note how the `operator new` has been intercepted and overridden by AddressSanitizer's new in `llvm/projects/compiler-rt/lib/asan/asan_new_delete.cc`.


---

# AddressSanitizer Bug Classes

AddressSanitizer doesn't detect uninitialized reads. More on this later.

---

# AddressSanitizer Bug Classes: Complete List

- Heap out-of-bounds
- Stack out-of-bounds
- Global out-of-bounds
- Use after free
- Use after return
- Memory leaks


---

# Undefined Behavior Sanitizer

In addition to the address sanitizer, we can use the undefined behavior sanitizer, which catch a different class of bugs.


---

# Undefined Behavior Sanitizer

Build undefined behavior instrumentation into binaries with

```bash
clang++ -std=c++14 -O0 -g -fno-omit-frame-pointer -fPIE -fsanitize=undefined -c asan_go_boom.cpp -o asan_go_boom.o
```


---

# Undefined Behavior Sanitizer

Floating point division by zero:

```cpp

double foo()
{
  return 0;
}

int main()
{
double x = 1/foo();
}
```

---

# Undefined Behavior Sanitizer


```bash
$ make clean && make UB=1 && ./ubsan_go_boom.x
ubsan_go_boom.cpp:35:15: runtime error: division by zero
```

---

# Undefined Behavior Sanitizer

Integer division by zero

```cpp
int bar()
{
return 0;
}

int main()
{
int y = 1/bar();
}
```


---

# Undefined Behavior Sanitizer

```bash
$ make clean && make UB=1 && ./ubsan_go_boom.x
ubsan_go_boom.cpp:36:12: runtime error: division by zero
```

---

# Undefined Behavior Sanitizer

Signed integer overflow:

```cpp
int k = std::numeric_limits<int>::max();
k += 1;
```

---

# Undefined Behavior Sanitizer

```bash
$ make clean && make UB=1 && ./ubsan_go_boom.x
ubsan_go_boom.cpp:28:5: runtime error: signed integer overflow: 2147483647 + 1 cannot be represented in type 'int'
```


--- 

# Undefined Behavior Sanitizer

Float cast overflow:

```cpp
double d = std::numeric_limits<double>::max();
float f = (float) d;
```


---

# Undefined Behavior Sanitizer

```bash
$ make clean && make UB=1 && ./ubsan_go_boom.x
ubsan_go_boom.cpp:11:21: runtime error: value 1.79769e+308 is outside the range of representable values of type 'float'
```


---


# Undefined Behavior Sanitizer

Null pointer operations are detected:

```cpp
int* p = nullptr;
int d = *p;
```


--- 

# Undefined Behavior Sanitizer


```bash
$ make clean && make UB=1 && ./ubsan_go_boom.x
ubsan_go_boom.cpp:38:11: runtime error: load of null pointer of type 'int'
```