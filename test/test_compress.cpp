#include <moor/archive_writer.hpp>

using namespace moor;

int main()
{
  auto compressor =  ArchiveWriter("test1.tar.gz", Format_pax, Compression_gzip);
  compressor.AddFile("ttt1");
  compressor.AddFile("ttt/test_compress");
  compressor.AddFile("test_compress");

  return 0;
}
