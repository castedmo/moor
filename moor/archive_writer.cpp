#include "archive_writer.hpp"

#include <archive.h>
#include <archive_entry.h>

#include <boost/filesystem.hpp>

#include <stdexcept>
#include <fstream>

#include <sys/stat.h>

using namespace moor;

int (*format_tab[])(archive*) = {archive_write_set_format_pax_restricted, archive_write_set_format_gnutar, 
                                 archive_write_set_format_zip, archive_write_set_format_7zip};
                          
int (*compression_tab[])(archive*) = {archive_write_set_compression_gzip, archive_write_set_compression_bzip2, 
                                      archive_write_set_compression_lzma};


void ArchiveWriter::checkError(const int _err_code, const bool _close_before_throw)
{
  if (_close_before_throw && _err_code == ARCHIVE_FATAL)
    Close();
  if (_err_code != ARCHIVE_OK && _err_code != ARCHIVE_WARN)
    throw std::runtime_error(archive_error_string(m_archive));
}
 

ArchiveWriter::ArchiveWriter(const std::string& _archive_file_name, const Formats& _format, const Compressions& _compression)
  : m_open (true), m_archive (archive_write_new()), m_entry (archive_entry_new())
  , m_archive_file_name (_archive_file_name), m_format(_format), m_compression(_compression)
{  
  //set archive format
  checkError((format_tab[m_format](m_archive)), true);    
  //set archive compression
  checkError((compression_tab[m_compression](m_archive)), true);  
  checkError(archive_write_open_filename(m_archive, m_archive_file_name.c_str()), true);
}

ArchiveWriter::~ArchiveWriter()
{
  Close();
}

void ArchiveWriter::addHeader(const std::string& _entry_name, const FileTypes _entry_type,
                              const unsigned int _size, const int _permission)
{
    m_entry = archive_entry_clear(m_entry);
    archive_entry_set_perm(m_entry, perm);    

}

void addHeader(const std::string& _file_path)
{
}

void ArchiveWriter::AddFile (const std::string& _file_path)
{
  if (boost::filesystem::exists(_file_path))
  {
    m_entry = archive_entry_clear(m_entry);
    archive_entry_set_pathname(m_entry, _file_path.c_str());
    struct stat native_stat;
    stat(_file_path.c_str(), &native_stat);
    archive_entry_copy_stat(m_entry, &native_stat);
    
    boost::filesystem::file_status stat = boost::filesystem::status(_file_path);
    boost::filesystem::perms perm = stat.permissions();
    archive_entry_set_perm(m_entry, perm);    
    
    if (stat.type() == boost::filesystem::directory_file)
    {
      archive_entry_set_filetype(m_entry, AE_IFDIR);      
      checkError(archive_write_header(m_archive, m_entry));
    }
    else if (stat.type() == boost::filesystem::regular_file)
    {
      archive_entry_set_filetype(m_entry, AE_IFREG);
      archive_entry_set_size(m_entry, boost::filesystem::file_size(_file_path));      
      checkError(archive_write_header(m_archive, m_entry));
      
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
      
    archive_write_finish_entry(m_archive);
  }
  else
    throw std::runtime_error("Entry file not found.");
}

void ArchiveWriter::AddDirectory(const std::string& _directory_name)
{
  m_entry = archive_entry_clear(m_entry);
  archive_entry_set_pathname(m_entry, _directory_name.c_str());
  archive_entry_set_filetype(m_entry, AE_IFDIR);
  checkError(archive_write_header(m_archive, m_entry));
  archive_write_finish_entry(m_archive);  
}

//template <class Iter>
void ArchiveWriter::AddFile (const Iter _entry_contents_begin, const Iter _entry_contents_end, const std::string& _entry_name);
{
  m_entry = archive_entry_clear(m_entry);
  archive_entry_set_pathname(m_entry, _entry_name.c_str());
  archive_entry_set_filetype(m_entry, AE_IFREG);
  archive_entry_set_size(m_entry, _entry_content.size());      
  checkError(archive_write_header(m_archive, m_entry));
  for (auto it = _entry_content
  archive_write_finish_entry(m_archive);  
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
