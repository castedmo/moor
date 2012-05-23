#include <moor/archive_writer.h>

using namespace moor;

int main()
{
  auto compressor =  ArchiveWriter(Format_pax, Compression_gzip);
  compressor.AddFile("");
  

  return 0;
}
