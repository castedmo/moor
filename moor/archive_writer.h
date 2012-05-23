#pragma once

#include <string>

#include "moor_defs.h"

struct archive;
struct archive_entry;

namespace moor
{
  class ArchiveWriter
  {
  public:
    ArchiveWriter(const Formats& _format, const Compressions& _compression);
    ~ArchiveWriter();
    
    void AddFile (const std::string& _file_path);
    void Close();
  
  private:
    bool m_open;
    archive* m_archive;
    archive_entry* m_entry;
    
    Formats m_format;
    Compressions m_compression;
  }; 
}
