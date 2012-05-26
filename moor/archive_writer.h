#pragma once

#include <string>

#include "moor_build_config.h"
#include "moor_defs.h"

struct archive;
struct archive_entry;

namespace moor
{
  class MOOR_API ArchiveWriter
  {
  public:
    ArchiveWriter(const std::string& _archive_file_name, const Formats& _format, const Compressions& _compression);
    ~ArchiveWriter();
    
    void AddFile (const std::string& _file_path);
    void Close();
  
  private:
    void checkError(const int _err_code, const bool _close_before_throw = false);
    
    bool m_open;
    archive* m_archive;
    archive_entry* m_entry;
    
    const std::string& m_archive_file_name;
    const Formats m_format;
    const Compressions m_compression;
  }; 
}
