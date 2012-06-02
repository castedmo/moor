#include "archive_writer.hpp"
#include "memory_writer_callback.hpp"

#include <archive.h>
#include <archive_entry.h>

#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>

#include <stdexcept>
#include <fstream>

#include <sys/stat.h>

using namespace moor;

int (*format_tab[])(archive*) = {archive_write_set_format_pax_restricted, archive_write_set_format_gnutar, 
                                 archive_write_set_format_zip, archive_write_set_format_7zip};
                          
int (*compression_tab[])(archive*) = {archive_write_set_compression_gzip, archive_write_set_compression_bzip2, 
                                      archive_write_set_compression_lzma};

int archive_file_type[] = {AE_IFREG, AE_IFDIR};

void ArchiveWriter::checkError(const int _err_code, const bool _close_before_throw)
{
  if (_close_before_throw && _err_code == ARCHIVE_FATAL)
    Close();
  if (_err_code != ARCHIVE_OK && _err_code != ARCHIVE_WARN)
    throw std::runtime_error(archive_error_string(m_archive));
}
 

ArchiveWriter::ArchiveWriter(const std::string& _archive_file_name, const Formats& _format, const Compressions& _compression)
  : m_open (true), m_archive (archive_write_new()), m_entry (archive_entry_new())
  , m_archive_file_name (_archive_file_name), m_out_buffer (NULL), m_format(_format), m_compression(_compression)
{  
  //set archive format
  checkError((format_tab[m_format](m_archive)), true);    
  //set archive compression
  checkError((compression_tab[m_compression](m_archive)), true);  
  checkError(archive_write_open_filename(m_archive, m_archive_file_name.c_str()), true);
}

ArchiveWriter::ArchiveWriter(std::list<unsigned char>& _out_buffer, const Formats& _format, const Compressions& _compression)
  : m_open(true), m_archive(archive_write_new()), m_entry (archive_entry_new())
  , m_archive_file_name (""), m_out_buffer (&_out_buffer), m_format(_format), m_compression(_compression)
{
  //set archive format
  checkError((format_tab[m_format](m_archive)), true);    
  //set archive compression
  checkError((compression_tab[m_compression](m_archive)), true);  
  checkError(write_open_memory(m_archive, m_out_buffer), true);
}

ArchiveWriter::~ArchiveWriter()
{
  Close();
}

void ArchiveWriter::addHeader(const std::string& _entry_name, const FileTypes _entry_type,
                              const long long _size, const int _permission)
{
  m_entry = archive_entry_clear(m_entry);
  archive_entry_set_pathname(m_entry, _entry_name.c_str());
  archive_entry_set_perm(m_entry, _permission);
  archive_entry_set_filetype(m_entry, archive_file_type[_entry_type]);
  archive_entry_set_size(m_entry, _size);
  checkError(archive_write_header(m_archive, m_entry));
}

void ArchiveWriter::addHeader(const std::string& _file_path)
{
  struct archive* a = archive_read_disk_new();
  BOOST_SCOPE_EXIT ( (&a) )
  {
    if(a != NULL)
    {
      archive_read_close(a);
      archive_read_free(a);
    }
  }
  BOOST_SCOPE_EXIT_END

  m_entry = archive_entry_clear(m_entry);
  archive_entry_set_pathname(m_entry, _file_path.c_str());
  archive_read_disk_entry_from_file(a, m_entry, 0, 0);
  checkError(archive_write_header(m_archive, m_entry));  
}

void ArchiveWriter::addContent(const char _byte)
{
  archive_write_data(m_archive, &_byte, 1);
}

void ArchiveWriter::addFinish()
{
  archive_write_finish_entry(m_archive);
}

void ArchiveWriter::AddFile (const std::string& _file_path)
{
  if (boost::filesystem::exists(_file_path))
  {
    boost::filesystem::file_status file_stat = boost::filesystem::status(_file_path);
    boost::filesystem::perms perm = file_stat.permissions();
    long long file_size = boost::filesystem::file_size(_file_path);

    addHeader(_file_path);

    if (file_stat.type() == boost::filesystem::regular_file)
    {
      std::fstream entry_file(_file_path.c_str(), std::ios::in);
      char buff[8192];
      while (entry_file.good())
      {
        entry_file.read(buff, 8192);
        archive_write_data(m_archive, buff, static_cast<size_t>(entry_file.gcount()));
      }
      entry_file.close();
    }
    else
      throw std::runtime_error("Entry file type not yet supported.");
      
    addFinish();
  }
  else
    throw std::runtime_error("Entry file not found.");
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
      archive_write_close(m_archive);
      archive_write_free (m_archive);
    }
    if (m_entry != NULL)
      archive_entry_free(m_entry);

    m_open = false;
  }
}
