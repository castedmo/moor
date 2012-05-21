#pragma once

#include <string>

struct archive;
struct archive_entry;

namespace moor
{
  class Compressor
  {
  public:
    Compressor();
    ~Compressor();
    
    AddFile (const std::string& _file_path);
    Close();
  
  private:
    bool m_open;
    archive* m_archive;
    archive_entry* m_entry; 
  }; 
}
