
#include "../LogStream.h"
#include "../../datetime/TimeStamp.h"

#include <stdio.h>

using namespace libcpp;

const int N = 1000000;

template<typename T>
void benchLogStream()
{
  TimeStamp start(TimeStamp::now());
  LogStream os;
  for (int i = 0; i < N; ++i)
  {
    os << (T)(i);
    os.resetBuffer();
  }
  TimeStamp end(TimeStamp::now());

  printf("benchLogStream %f\n", timeInterval(end, start));
}

int main()
{
  puts("int");
  benchLogStream<int>();

  puts("double");
  benchLogStream<double>();

  puts("int64_t");
  benchLogStream<int64_t>();

  puts("void*");
  benchLogStream<void*>();

}