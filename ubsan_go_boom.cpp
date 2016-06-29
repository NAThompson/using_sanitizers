#include <limits>

void nulluse(int * p)
{
  int d = *p;
}

void floatoverflow()
{
  double d = std::numeric_limits<double>::max();
  float f = (float) d;
}


double foo()
{
  return 0;
}

int bar()
{
  return 0;
}

void overflow()
{
  int k = std::numeric_limits<int>::max();
  k += 1;
}

int main()
{
  //floatoverflow();
  //overflow();
  //double x = 1/foo();
  //int y = 1/bar();
  int* p = nullptr;
  int d = *p;
  //nulluse(p);
}
