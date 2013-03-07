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
