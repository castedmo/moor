#pragma once

#include <list>

#if defined (_WIN64)
  typedef long long ssize_t
#elif defined (_WIN32)
  typedef long ssize_t
#endif

struct archive;
  
namespace moor
{
  int write_open_memory(struct archive *a, std::list<unsigned char>* _buff);
}
