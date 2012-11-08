#pragma once

#include <vector>

#if defined (_WIN64)
  typedef long long ssize_t;
#elif defined (_WIN32)
  typedef long ssize_t;
#endif

struct archive;

namespace moor
{
  int read_open_memory(struct archive *a, std::vector<unsigned char>* _buff
    , const long long _read_size = 10240ll);
}

