#include <moor/archive_writer.hpp>
#include <list>
#include <vector>
#include <iostream>
using namespace moor;

int main()
{
  ArchiveWriter compressor("test1.tar.gz", Format_pax, Compression_gzip);
  //compressor.AddFile("ttt1");
  //compressor.AddFile("ttt/test_compress");
  compressor.AddFile("test_compress");
  compressor.AddDirectory("mem_dir");
  char a[] = {64, 65, 66, 67, 68};
  std::list<char> l(10, 'A');
  std::vector<char> v(10, 'B');

  compressor.AddFile("arary", a, a+10);
  compressor.AddFile("list", l.begin(), l.end());
  compressor.AddFile("vector", v.begin(), v.end());

  return 0;
}
