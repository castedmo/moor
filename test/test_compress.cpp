#include <moor/compressor.h>

using namespace moor;

int main()
{
  auto compressor =  Compressor(Format_pax, Compression_gzip);
  compressor.AddFile("");
  

  return 0;
}
