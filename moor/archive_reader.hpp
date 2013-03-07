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
#include <vector>
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
    ArchiveReader(unsigned char*_in_buffer, const size_t _size);
    ArchiveReader(std::vector<unsigned char>&& _in_buffer);
    ~ArchiveReader();

    //returns FALSE at EOF
    bool ExtractNext (const std::string& _root_path);
    //retuns empty filename at EOF
    std::pair<std::string, std::vector<unsigned char>> ExtractNext();

  private:
    void init();
    void checkError(const int _err_code, const bool _close_before_throw=false);
    void close();

    archive* m_archive;
    bool m_open;

    const std::string m_archive_file_name;
    std::vector<unsigned char> m_in_buffer;
  };
}

