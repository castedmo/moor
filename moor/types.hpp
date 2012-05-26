#pragma once

namespace moor
{
  enum Formats
  {
    Format_pax = 0,
    Format_tar = 1,
    Format_ZIP = 2,
    Format_7Zip = 3
  };
  
  enum Compressions
  {
    Compression_gzip = 0,
    Compression_bzip2 = 1,
    Compression_lzma = 2
  };

  enum FileTypes
  {
    FileType_Regular = 0,
    FileType_Directory = 1
  };
}
