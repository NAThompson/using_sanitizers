#include <ostream>


int use_after_free()
{
    int * p = new int[1];
    delete[] p;
    return p[0];
}

void uninitialized_reads_not_detected()
{
    int p[0];
    std::ostream cnull(nullptr);
    cnull << p[0];
}

void leak()
{
    int * p = new int[1];
    return;
}

void heap_oob()
{
    int* p = new int[1];

    std::ostream cnull(nullptr);
    cnull << p[1];

    delete[] p;
}

void stack_oob()
{
    int p[1];
    std::ostream cnull(nullptr);
    cnull << p[1];
}


int main()
{
    use_after_free();
    //uninitialized_reads_not_detected();
    //leak();
    //heap_oob();
    //stack_oob();
}
