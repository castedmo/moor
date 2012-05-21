#include "compressor.h"

#include <archive.h>
#include <archive_entry.h>

using namespace moor;

Compressor::Compressor()
  : m_open (true), m_archive (archive_write_new()), m_entry (archive_entry_new())
{  
}

Compressor::~Compressor()
{
  Close();
}

Compressor::AddFile (const std::string& _file_path)
{
}

Compressor::Close()
{
  if (m_open)
  {
    if (m_archive != NULL)
    {
      archive_write_close(m_archive);
      arcihve_write_free (m_archive);
    }
    if (m_entry != NULL)
      archive_entry_free(m_entry);

    m_open = false;
  }
}
