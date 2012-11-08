#pragma once
#include <string>
#include <list>
#include <utility>

#include "moor_build_config.hpp"
#include "types.hpp"

struct archive;
struct archive_entry;

namespace moor
{
  class MOOR_API ArchiveReader
  {
  public:
    ArchiveReader(const std::string& _archive_file_name);
    ArchiveReader(std::list<unsigned char>& _in_buffer);
    ArchiveReader(std::list<unsigned char>&& _in_buffer);
    ~ArchiveReader();

    //returns FALSE at EOF
    bool ExtractNext (const std::string& _root_path);
    //retuns empty filename at EOF
    std::pair<std::string, std::list<unsigned char>> ExtractNext();

  private:
    void init();
    void checkError(const int _err_code, const bool _close_before_throw=false);
    void close();

    archive* m_archive;
    bool m_open;

    const std::string m_archive_file_name;
    std::list<unsigned char> m_in_buffer;
  };
}

