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

#include "archive_writer.hpp"
#include "memory_writer_callback.hpp"
#include "scope_exit.hpp"

#include <archive.h>
#include <archive_entry.h>

#include <stdexcept>
#include <fstream>

#include <sys/stat.h>

using namespace moor;

int (*format_tab[])(archive*)
    = {archive_write_set_format_pax_restricted, archive_write_set_format_gnutar
        , archive_write_set_format_zip, archive_write_set_format_7zip};

int (*compression_tab[])(archive*)
    = {archive_write_set_compression_none, archive_write_set_compression_gzip,
      archive_write_set_compression_bzip2 , archive_write_set_compression_lzma};

int archive_file_type[] = {AE_IFREG, AE_IFDIR};

void ArchiveWriter::checkError(const int _err_code
  , const bool _close_before_throw)
{
  std::string error_str;
  if (_err_code != ARCHIVE_OK && _err_code != ARCHIVE_WARN)
    error_str = archive_error_string(m_archive);
  if (_close_before_throw && _err_code == ARCHIVE_FATAL)
    Close();
  if (_err_code != ARCHIVE_OK && _err_code != ARCHIVE_WARN)
    throw std::runtime_error(error_str);
}


ArchiveWriter::ArchiveWriter(const std::string& _archive_file_name
    , const Formats& _format, const Compressions& _compression)
  : m_open (true), m_archive (archive_write_new()), m_entry (archive_entry_new())
  , m_archive_file_name (_archive_file_name)
   , m_format(_format), m_compression(_compression)
{
  //set archive format
  checkError((format_tab[m_format](m_archive)), true);
  //set archive compression
  checkError((compression_tab[m_compression](m_archive)), true);
  checkError(archive_write_open_filename(m_archive, m_archive_file_name.c_str())
      , true);
}

ArchiveWriter::ArchiveWriter(std::list<unsigned char>& _out_buffer
    , const Formats& _format, const Compressions& _compression)
  : m_open(true), m_archive(archive_write_new()), m_entry (archive_entry_new())
  , m_archive_file_name (""), m_format(_format)
  , m_compression(_compression)
{
  //set archive format
  checkError((format_tab[m_format](m_archive)), true);
  //set archive compression
  checkError((compression_tab[m_compression](m_archive)), true);
  checkError(write_open_memory(m_archive, &_out_buffer), true);
}

ArchiveWriter::ArchiveWriter(unsigned char* _out_buffer, size_t* _size
    , const Formats& _format, const Compressions& _compression)
  : m_open(true), m_archive(archive_write_new()), m_entry (archive_entry_new())
  , m_archive_file_name (""), m_format(_format)
  , m_compression(_compression)
{
  //set archive format
  checkError((format_tab[m_format](m_archive)), true);
  //set archive compression
  checkError((compression_tab[m_compression](m_archive)), true);
  checkError(archive_write_open_memory(m_archive, _out_buffer, *_size, _size)
   , true);
}

ArchiveWriter::ArchiveWriter(const Formats& _format, const Compressions& _compression)
  : m_archive(archive_write_new()), m_entry (archive_entry_new())
  , m_archive_file_name (""), m_format(_format), m_compression(_compression)
{
  //set archive format
  checkError((format_tab[m_format](m_archive)), true);
  //set archive compression
  checkError((compression_tab[m_compression](m_archive)), true);
}
void ArchiveWriter::Open(const std::string& _archive_file_name)
{
  checkError(archive_write_open_filename(m_archive, _archive_file_name.c_str())
      , false);
  m_open = true;
}
void ArchiveWriter::Open(std::list<unsigned char>& _out_buffer)
{
  checkError(write_open_memory(m_archive, &_out_buffer), false);
  m_open = true;
}
void ArchiveWriter::Open(unsigned char * _out_buffer, size_t* _size)
{
  checkError(archive_write_open_memory(m_archive, _out_buffer, *_size, _size)
   , false);
  m_open = true;
}

ArchiveWriter::~ArchiveWriter()
{
  try
  {
    Close();
  }
  catch (...)
  {}
}

void ArchiveWriter::SetFormatOption(const std::string& _option, const std::string& _value)
{
  checkError(archive_write_set_format_option(m_archive, NULL, _option.c_str(), _value.c_str()), false);
}

void ArchiveWriter::addHeader(const std::string& _entry_name
  , const FileTypes _entry_type, const long long _size, const int _permission)
{
  m_entry = archive_entry_clear(m_entry);
  archive_entry_set_pathname(m_entry, _entry_name.c_str());
  archive_entry_set_perm(m_entry, _permission);
  archive_entry_set_filetype(m_entry, archive_file_type[_entry_type]);
  archive_entry_set_size(m_entry, _size);
  checkError(archive_write_header(m_archive, m_entry));
}

void ArchiveWriter::addHeader(const std::string& _file_path, const std::string& _entry_name)
{
  struct archive* a = archive_read_disk_new();
  ScopeExit se([&a]
  {
    if(a != NULL)
    {
      archive_read_close(a);
      archive_read_free(a);
    }
  });

  m_entry = archive_entry_clear(m_entry);
  archive_entry_set_pathname(m_entry, _entry_name.c_str());
  archive_entry_copy_sourcepath(m_entry, _file_path.c_str());
  checkError(archive_read_disk_entry_from_file(a, m_entry, -1, 0));
  checkError(archive_write_header(m_archive, m_entry));
}

void ArchiveWriter::addContent(const char _byte)
{
  archive_write_data(m_archive, &_byte, 1);
}

void ArchiveWriter::addContent(const char* _bytes, const unsigned long long _size)
{
  archive_write_data(m_archive, _bytes, static_cast<size_t>(_size));
}

void ArchiveWriter::addFinish()
{
  checkError(archive_write_finish_entry(m_archive), false);
}

void ArchiveWriter::AddFile (const std::string& _file_path, const std::string& _entry_name)
{
  struct stat file_stat;
  if (stat(_file_path.c_str(), &file_stat) < 0)
  {
    switch (errno)
    {
    case ENOENT:
      throw std::runtime_error("Entry file not found.");
      break;
    default:
      throw std::runtime_error("Entry file is not valid.");
      break;
    }
  }
  addHeader(_file_path, _entry_name.empty() ? _file_path : _entry_name);

  if (file_stat.st_mode & S_IFREG)
  {
    std::fstream entry_file(_file_path.c_str(), std::ios::in | std::ios::binary);
    char buff[8192];
    while (entry_file.good())
    {
      entry_file.read(buff, 8192);
      archive_write_data(m_archive, buff
        , static_cast<size_t>(entry_file.gcount()));
    }
    entry_file.close();
  }
  else
    throw std::runtime_error("Entry file type not yet supported.");

  addFinish();
}
void ArchiveWriter::AddFile (const std::string& _entry_name
  , const unsigned char * _data , const unsigned long long _size)
{
  addHeader(_entry_name, FileType_Regular, _size);
  addContent((char*)_data, _size);
  addFinish();
}

void ArchiveWriter::AddDirectory(const std::string& _directory_name)
{
  addHeader(_directory_name, FileType_Directory, 0777);
  addFinish();
}

void ArchiveWriter::Close()
{
  if (m_open)
  {
    if (m_archive != NULL)
    {
      checkError(archive_write_close(m_archive));
      checkError(archive_write_free(m_archive));
    }
    if (m_entry != NULL)
      archive_entry_free(m_entry);

    m_open = false;
  }
}
