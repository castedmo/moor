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

#include "archive_reader.hpp"
#include "scope_exit.hpp"

#include <archive.h>
#include <archive_entry.h>
#include <stdexcept>
#include <cerrno>

using namespace moor;

ArchiveReader::ArchiveReader(const std::string& _archive_file_name)
  :m_archive_file_name(_archive_file_name), m_archive(archive_read_new())
   , m_open(true)
{
  struct stat file_stat;
  if (stat(_archive_file_name.c_str(), &file_stat) < 0)
  {
    switch (errno)
    {
    case ENOENT:
      throw std::runtime_error("Archive file not found.");
      break;
    default:
      throw std::runtime_error("Archive file is not valid.");
      break;
    }
  }

  init();
  checkError(archive_read_open_file(m_archive, m_archive_file_name.c_str()
    , 10240), true);

}

ArchiveReader::ArchiveReader(unsigned char* _in_buffer, const size_t _size)
  :m_archive_file_name(""), m_archive(archive_read_new()) , m_open(true)
{
  init();
  checkError(archive_read_open_memory(m_archive, _in_buffer, _size), true);
}

ArchiveReader::ArchiveReader(std::vector<unsigned char>&& _in_buffer)
  :m_archive_file_name(""), m_in_buffer(std::move(_in_buffer))
   , m_archive(archive_read_new()), m_open(true)
{
  init();
  checkError(archive_read_open_memory(m_archive, &*m_in_buffer.begin()
    , m_in_buffer.size()), true);
}

void ArchiveReader::init()
{
  checkError(archive_read_support_format_all(m_archive), true);
  checkError(archive_read_support_compression_all(m_archive), true);
}

ArchiveReader::~ArchiveReader()
{
  close();
}

int copy_data(struct archive *ar, struct archive *aw)
{
  int r;
  const void *buff;
  size_t size;
  __LA_INT64_T offset;

  for (;;)
  {
    r = archive_read_data_block(ar, &buff, &size, &offset);
    if (r == ARCHIVE_EOF)
      return (ARCHIVE_OK);
    if (r != ARCHIVE_OK)
      return (r);
    r = archive_write_data_block(aw, buff, size, offset);
    if (r != ARCHIVE_OK)
      return (r);
  }
}

bool ArchiveReader::ExtractNext (const std::string& _root_path)
{
  int flags;

  /* Select which attributes we want to restore. */
  flags = ARCHIVE_EXTRACT_TIME;
  flags |= ARCHIVE_EXTRACT_PERM;
  flags |= ARCHIVE_EXTRACT_ACL;
  flags |= ARCHIVE_EXTRACT_FFLAGS;

  struct archive* a = archive_write_disk_new();
  archive_write_disk_set_options(a, flags);
  archive_write_disk_set_standard_lookup(a);

  struct archive_entry* entry;
  auto r = archive_read_next_header(m_archive, &entry);
  ScopeExit se([&a]
  {
    if(a != NULL)
    {
      archive_write_finish_entry(a);
      archive_write_close(a);
      archive_write_free(a);
    }
  });

  if (r == ARCHIVE_EOF)
    return false;
  else
    checkError(r);

  archive_entry_set_pathname(entry,
    (_root_path + "/" + archive_entry_pathname(entry)).c_str());
  checkError(archive_write_header(a, entry));
  if (archive_entry_size(entry) > 0)
    checkError(copy_data(m_archive, a));

  return true;
}

std::pair<std::string, std::vector<unsigned char>> ArchiveReader::ExtractNext()
{
  auto result = std::make_pair(std::string(""), std::vector<unsigned char>());

  struct archive_entry* entry;
  auto r = archive_read_next_header(m_archive, &entry);
  if (r == ARCHIVE_EOF)
    return result;
  else
    checkError(r);

  result.first = archive_entry_pathname(entry);
  auto entry_size = archive_entry_size(entry);
  if (entry_size > 0)
  {
    int r;
    size_t read_index = 0;
    result.second.resize(entry_size);
    for (;;)
    {
      r = archive_read_data(m_archive, &result.second[read_index]
        , result.second.size() - read_index);
      if (r == 0)
        break;
      if (r < ARCHIVE_OK)
        checkError (r);

      read_index += r;
      if (read_index == entry_size)
        break;
    }
  }

  return result;
}

void ArchiveReader::checkError(const int _err_code
  , const bool _close_before_throw)
{
  std::string error_str;
  if (_err_code != ARCHIVE_OK && _err_code != ARCHIVE_WARN)
    error_str = archive_error_string(m_archive);
  if (_close_before_throw && _err_code == ARCHIVE_FATAL)
    close();
  if (_err_code != ARCHIVE_OK && _err_code != ARCHIVE_WARN)
    throw std::runtime_error(error_str);
}

void ArchiveReader::close()
{
  if (m_open)
  {
    if (m_archive != NULL)
    {
      archive_read_close(m_archive);
      archive_read_free (m_archive);
    }

    m_open = false;
  }
}
