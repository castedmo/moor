#include "memory_reader_callback.hpp"

#include <archive.h>
#include <stdexcept>

using namespace moor;

struct read_memory_data
{
	//unsigned char	*start;
	//unsigned char	*p;
	//unsigned char	*end;
	ssize_t	 read_size;
  size_t read_index;
  std::vector<unsigned char> * buff;
};

static int	moor_memory_read_close(struct archive *, void *);
static int	moor_memory_read_open(struct archive *, void *);
static int64_t	moor_memory_read_seek(struct archive *, void *,
  int64_t offset, int whence);
static int64_t	moor_memory_read_skip(struct archive *, void *,
  int64_t request);
static ssize_t	moor_memory_read(struct archive *, void *, const void **buff);

int moor::read_open_memory(struct archive *a, std::vector<unsigned char>* _buff
  , const long long _read_size)
{
	struct read_memory_data *mine = new read_memory_data;

	if (mine == NULL)
    throw std::runtime_error("no memory");

  mine->buff = _buff;
  mine->read_index = 0;
  mine->read_size = (ssize_t)_read_size;
	archive_read_set_open_callback(a, moor_memory_read_open);
	archive_read_set_read_callback(a, moor_memory_read);
	archive_read_set_seek_callback(a, moor_memory_read_seek);
	archive_read_set_skip_callback(a, moor_memory_read_skip);
	archive_read_set_close_callback(a, moor_memory_read_close);
	archive_read_set_callback_data(a, mine);
	return (archive_read_open1(a));
}

static int moor_memory_read_open(struct archive *, void *)
{
	return (ARCHIVE_OK);
}

static ssize_t moor_memory_read(struct archive *, void *client_data, const void **buff)
{
	struct read_memory_data *mine = (struct read_memory_data *)client_data;

	*buff = &mine->buff[mine->read_index];
  ssize_t size = mine->buff->size() - mine->read_index;
  if (size > mine->read_size)
    size = mine->read_size;
  mine->read_index += size;
	return size;
}

static int64_t moor_memory_read_skip(struct archive *, void *client_data, int64_t skip)
{
	struct read_memory_data *mine = (struct read_memory_data *)client_data;

	if ((int64_t)skip > (int64_t)(mine->buff->size() - mine->read_index))
		skip = mine->buff->size() - mine->read_index;
  /* Round down to block size. */
  skip /= mine->read_index;
  skip *= mine->read_index;
	mine->read_index += skip;
	return (skip);
}

static int64_t moor_memory_read_seek(struct archive *, void *client_data, int64_t offset, int whence)
{
	struct read_memory_data *mine = (struct read_memory_data *)client_data;

	switch (whence)
  {
	case SEEK_SET:
		mine->read_index = offset;
		break;
	case SEEK_CUR:
		mine->read_index += offset;
		break;
	case SEEK_END:
		mine->read_index = mine->buff->size() + offset;
		break;
	default:
		return ARCHIVE_FATAL;
	}
	if (mine->read_index < 0)
  {
		mine->read_index = 0;
		return ARCHIVE_FAILED;
	}
	if (mine->read_index >= mine->buff->size())
  {
		mine->read_index = mine->buff->size() - 1;
		return ARCHIVE_FAILED;
	}
	return (mine->read_index);
}

static int moor_memory_read_close(struct archive *, void *client_data)
{
	struct read_memory_data *mine = (struct read_memory_data *)client_data;
  delete mine;
	return (ARCHIVE_OK);
}
