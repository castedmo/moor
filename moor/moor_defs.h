#pragma once

namespace moor
{
  enum Formats
  {
    Format_pax = 1,
    Format_tar = 2,
    Format_ZIP = 3,
    Format_7Zip = 4
  };
  
  enum Compressions
  {
    Compression_gzip = 1,
    Compression_bzip2 = 2,
    Compression_lzma = 3
  };
}
