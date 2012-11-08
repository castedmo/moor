#include "archive_reader.hpp"

#include <archive.h>
#include <archive_entry.h>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

using namespace moor;

ArchiveReader::ArchiveReader(const std::string& _archive_file_name)
  :m_archive_file_name(_archive_file_name), m_archive(archive_read_new())
   , m_open(true)
{
  if (!boost::filesystem::exists(m_archive_file_name))
    throw std::runtime_error("Archive file not found.");

  init();
  checkError(archive_read_open_file(m_archive, m_archive_file_name.c_str()
    , 10240), true);

}

ArchiveReader::ArchiveReader(std::vector<unsigned char>& _in_buffer)
  :m_archive_file_name(""), m_in_buffer(_in_buffer)
   , m_archive(archive_read_new()) , m_open(true)
{
  init();
  checkError(archive_read_open_memory(m_archive, &*m_in_buffer.begin()
    , m_in_buffer.size()), true);
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
  BOOST_SCOPE_EXIT (&a)
  {
    if(a != NULL)
    {
      archive_write_finish_entry(a);
      archive_write_close(a);
      archive_write_free(a);
    }
  }
  BOOST_SCOPE_EXIT_END
  if (r == ARCHIVE_EOF)
    return false;
  else
    checkError(r);

  archive_entry_set_pathname(entry,
      (boost::filesystem::path(_root_path) /
      archive_entry_pathname(entry)).string().c_str());
  checkError(archive_write_header(a, entry));
  if (archive_entry_size(entry) > 0)
    checkError(copy_data(m_archive, a));

  return true;
}

std::pair<std::string, std::list<unsigned char>> ArchiveReader::ExtractNext()
{
  auto result = std::make_pair(std::string(""), std::list<unsigned char>());

  struct archive_entry* entry;
  auto r = archive_read_next_header(m_archive, &entry);
  if (r == ARCHIVE_EOF)
    return result;
  else
    checkError(r);

  result.first = archive_entry_pathname(entry);
  if (archive_entry_size(entry) > 0)
  {
    int r;
    char c;
    for (;;)
    {
      r = archive_read_data(m_archive, &c, 1);
      if (r == 0)
        break;
      if (r < ARCHIVE_OK)
        checkError (r);

      result.second.push_back(c);
    }
  }

  return result;
}

void ArchiveReader::checkError(const int _err_code
  , const bool _close_before_throw)
{
  if (_close_before_throw && _err_code == ARCHIVE_FATAL)
    close();
  if (_err_code != ARCHIVE_OK && _err_code != ARCHIVE_WARN)
    throw std::runtime_error(archive_error_string(m_archive));
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
