#include "fdstream.h"

_fdstream::_fdstream()
            : _filebuf(NULL), _file_descriptor(-1)
    { }

_fdstream::_fdstream(int file_descriptor, std::ios_base::openmode openmode)
            : _filebuf(NULL), _file_descriptor(file_descriptor), _openmode(openmode)
    {
        open(file_descriptor, openmode);
    }

    std::ios_base::openmode _fdstream::openmode() const { return _openmode; }

    void _fdstream::open(int file_descriptor, std::ios_base::openmode openmode)
    {
        if (!_filebuf)
            // We create a C++ stream from a file descriptor
            // stdio_filebuf is not synced with stdio.
            // From GCC 3.4.0 on exists in addition stdio_sync_filebuf
            // You can also create the filebuf from a FILE* with
            // FILE* f = fdopen(file_descriptor, mode);
            _filebuf = new __gnu_cxx::stdio_filebuf<char> (file_descriptor,
                                                           openmode);
    }

    _fdstream::~_fdstream()
    {
        close(_file_descriptor);
        delete _filebuf;
    }

    ifdstream::ifdstream() :
            _fdstream(), _stream(NULL)
    { }

    ifdstream::ifdstream(int file_descriptor) :
            _fdstream(file_descriptor, std::ios_base::in)
    {
        _stream = new std::istream (_filebuf);
    }

    void ifdstream::open(int file_descriptor)
    {
        if (!_stream)
        {
            _fdstream::open(file_descriptor, std::ios_base::in);
            _stream = new std::istream (_filebuf);
        }
    }

    ifdstream& ifdstream::operator>> (std::string& str)
    {
        (*_stream) >> str;

        return *this;
    }

    size_t ifdstream::getline (char* s, std::streamsize n)
    {
        return (getline(s, n, '\n'));
    }

    size_t ifdstream::getline (char* s, std::streamsize n, char delim)
    {
        int i = 0;
        do
        {
            s[i] = _stream->get();
            i++;
        } while(i < n-1 && s[i-1] != delim && s[i-1] != '\0');

        s[i-1] = '\0'; // overwrite the delimiter given with std::string end

        return i-1;
    }

ifdstream::~ifdstream()
    {
        delete _stream;
    }


    ofdstream::ofdstream()
            : _fdstream(), _stream(NULL)
    { }

ofdstream::ofdstream(int file_descriptor)
            : _fdstream(file_descriptor, std::ios_base::out)
    {
        _stream = new std::ostream (_filebuf);
    }

    void ofdstream::open(int file_descriptor)
    {
        if (!_stream)
        {
            _fdstream::open(file_descriptor, std::ios_base::out);
            _stream = new std::ostream (_filebuf);
        }
    }


    ofdstream& ofdstream::operator<< (const std::string& str)
    {
        if (_stream->good())
        {
            (*_stream) << str;
        }

        _stream->flush();
        return *this;
    }

ofdstream::~ofdstream()
    {
        delete _stream;
    }


size_t getline (ifdstream& ifds, std::string& str)
{
    char *tmp = new char[BUFFER_SIZE];
    size_t ret = ifds.getline(tmp, BUFFER_SIZE);
    str = tmp;
    return ret;
}
