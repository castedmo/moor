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
    
    void AddFile (const std::string& _file_path);
    void Close();
  
  private:
    bool m_open;
    archive* m_archive;
    archive_entry* m_entry; 
  }; 
}
