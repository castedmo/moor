#include "archive_writer.h"

#include <archive.h>
#include <archive_entry.h>

using namespace moor;

int (*format_ptr[])(archive*) = {archive_write_set_format_pax_restricted, archive_write_set_format_gnutar, 
                                 archive_write_set_format_zip, archive_write_set_format_7zip};
                          
int (*compression_ptr[])(archive*) = {archive_write_set_compression_gzip, archive_write_set_compression_bzip2, 
                                      archive_write_set_compression_lzma};

ArchiveWriter::ArchiveWriter(const Formats& _format, const Compressions& _compression)
  : m_open (true), m_archive (archive_write_new()), m_entry (archive_entry_new())
  , m_format(_format), m_compression(_compression)
{  
}

ArchiveWriter::~ArchiveWriter()
{
  Close();
}

void ArchiveWriter::AddFile (const std::string& _file_path)
{
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
