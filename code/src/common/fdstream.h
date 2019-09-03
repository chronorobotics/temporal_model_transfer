/* This is a basic example of creating a C++ stream from a file
 * descriptor and how to read/write from/to it
 *
 * Needs GCC's libstdc++ and a POSIX environment
 *
 * Author: Diego Barrios Romero
 * Public domain Copyleft 2011
 */


#ifndef _FDSTREAM_
#define _FDSTREAM_

#include <iostream>
#if defined(__GLIBCXX__) || defined(__GLIBCPP__)
#include <unistd.h>
#include <ext/stdio_filebuf.h>

#define BUFFER_SIZE (512)

class _fdstream
{
protected:
  _fdstream();
  _fdstream(int file_descriptor, std::ios_base::openmode openmode);

  std::ios_base::openmode openmode() const;

  void open(int file_descriptor, std::ios_base::openmode openmode);

  virtual ~_fdstream();

  __gnu_cxx::stdio_filebuf<char>* _filebuf;
private:
  int _file_descriptor;
  std::ios_base::openmode _openmode;
};

class ifdstream : public _fdstream
{
public:
  ifdstream();

  ifdstream(int file_descriptor);

  void open(int file_descriptor);

  ifdstream& operator>> (std::string& str);

  size_t getline (char* s, std::streamsize n);

  size_t getline (char* s, std::streamsize n, char delim);

  ~ifdstream();

private:
  std::istream* _stream;
};

class ofdstream : public _fdstream
{
public:
  ofdstream();
  ofdstream(int file_descriptor);

  void open(int file_descriptor);

  ofdstream& operator<< (const std::string& str);

  ~ofdstream();

private:
  std::ostream* _stream;
};

size_t getline (ifdstream& ifds, std::string& str);

#else
# error "Not supported"
#endif

#endif // _FDSTREAM_

