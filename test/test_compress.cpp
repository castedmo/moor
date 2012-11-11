#include <moor/archive_writer.hpp>
#include <moor/archive_reader.hpp>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
using namespace moor;

int main()
{
  std::list<unsigned char> lout;
  //ArchiveWriter compressor("test1.tar.gz", Format_pax, Compression_gzip);
  ArchiveWriter compressor(lout, Format_pax, Compression_gzip);
  //compressor.AddFile("ttt1");
  //compressor.AddFile("ttt/test_compress");
  compressor.AddFile("test_compress");
  compressor.AddDirectory("mem_dir");
  char a[] = {64, 65, 66, 67, 68};
  std::list<char> l(10, 'A');
  std::vector<char> v(10, 'B');

  compressor.AddFile("mem_dir/arary", a, a+10);
  compressor.AddFile("list", l.begin(), l.end());
  compressor.AddFile("vector", v.begin(), v.end());


  compressor.Close();
  std::ofstream of("test2.tar.gz", std::ios::binary);
  for (auto a = lout.begin(); a != lout.end(); a++)
    of << *a;
  of.close();

  ArchiveReader reader1("test2.tar.gz");
  std::ifstream iff ("test2.tar.gz", std::ios::binary);
  iff.seekg(0, std::ios::end);
  auto size = iff.tellg();
  iff.seekg(0, std::ios::beg);
  std::vector<unsigned char> ff(size);

  while(iff.good())
    iff.read((char*)&*ff.begin(), size);
  ArchiveReader reader(std::move(ff));
  auto data = reader.ExtractNext();
  while(data.first.length() > 0)
  {
    std::cout << data.first << " : " << data.second.size()<< std::endl;
    data = reader.ExtractNext();
  }
  data = reader1.ExtractNext();
  while(data.first.length() > 0)
  {
    std::cout << data.first << " : " << data.second.size()<< std::endl;
    data = reader1.ExtractNext();
  }

  return 0;
}
