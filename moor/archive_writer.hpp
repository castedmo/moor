/*
 * Copyright (c) 2013 Mohammad Mehdi Saboorian
 *
 * This is part of moor, a wrapper for libarchive
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <string>
#include <iterator>
#include <list>

#include "moor_build_config.hpp"
#include "types.hpp"

struct archive;
struct archive_entry;

namespace moor
{
  class MOOR_API ArchiveWriter
  {
  public:
    //deprecated
    ArchiveWriter(const std::string& _archive_file_name, const Formats& _format,
                  const Compressions& _compression);
    //deprecated
    ArchiveWriter(std::list<unsigned char>& _out_buffer, const Formats& _format,
        const Compressions& _compression);
    //deprecated
    ArchiveWriter(unsigned char * _out_buffer, size_t* _size
      , const Formats& _format, const Compressions& _compression);
    ArchiveWriter(const Formats& _format, const Compressions& _compression);
    void Open(const std::string& _archive_file_name);
    void Open(std::list<unsigned char>& _out_buffer);
    void Open(unsigned char * _out_buffer, size_t* _size);
    ~ArchiveWriter();

    void SetFormatOption(const std::string& _option, const std::string& _value);

    void AddFile (const std::string& _file_path, const std::string& _entry_name="");

    template <class Iter>
    void AddFile (const std::string& _entry_name, const Iter _entry_contents_begin
        , const Iter _entry_contents_end, long long _size = -1);
    void AddFile (const std::string& _entry_name, const unsigned char * _data
      , const unsigned long long _size);
    void AddDirectory(const std::string& _directory_name);
    void Close();

  private:
    void checkError(const int _err_code, const bool _close_before_throw = false);
    void addHeader(const std::string& _entry_name, const FileTypes _entry_type,
                   const long long _size = 0, const int _permission = 0644);
    void addHeader(const std::string& _file_path, const std::string& _entry_name="");
    void addContent(const char _byte);
    void addContent(const char* _bytes, const unsigned long long _size);
    void addFinish();

    bool m_open;
    archive* m_archive;
    archive_entry* m_entry;

    const std::string m_archive_file_name;
    //std::list<unsigned char>* m_out_buffer;
    const Formats m_format;
    const Compressions m_compression;
  };

  template <class Iter>
  void ArchiveWriter::AddFile (const std::string& _entry_name
      , const Iter _entry_contents_begin, const Iter _entry_contents_end
      , long long _size)
  {
    long long size = _size > -1 ? _size :
        std::distance(_entry_contents_begin, _entry_contents_end);
    addHeader(_entry_name, FileType_Regular, size);
    for (Iter it = _entry_contents_begin; it != _entry_contents_end; it++)
      addContent(*it);
    addFinish();
  }

}
