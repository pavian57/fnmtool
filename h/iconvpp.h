#ifndef ICONVPP_H_INCLUDED
#define ICONVPP_H_INCLUDED
#include <iconv.h>
#include <stdexcept>
#include <cerrno>

class converter
{
public:
    converter(const std::string& out_encode,
              const std::string& in_encode,
              bool ignore_error = false,
              size_t buf_size = 1024)
        : ignore_error_(ignore_error),
          buf_size_(buf_size)
    {
        if (buf_size == 0)
        {
            throw std::runtime_error("buffer size must be greater than zero");
        }

        iconv_t conv = ::iconv_open(out_encode.c_str(), in_encode.c_str());
        if (conv == (iconv_t)-1)
        {
            if (errno == EINVAL)
                throw std::runtime_error("not supported from " + in_encode + " to " + out_encode);
            else
                throw std::runtime_error("unknown error");
        }
        iconv_ = conv;
    }

    ~converter()
    {
        iconv_close(iconv_);
    }

    void convert(const std::string& input, std::string& output) const
    {
        // copy the string to a buffer as iconv function requires a non-const char pointer
        std::vector<char> in_buf(input.begin(), input.end());
        char* src_ptr = &in_buf[0];
        size_t src_size = input.size();

        std::vector<char> buf(buf_size_);
        std::string dst;
        while (0 < src_size)
        {
            char* dst_ptr = &buf[0];
            size_t dst_size = buf.size();
            size_t res = ::iconv(iconv_, &src_ptr, &src_size, &dst_ptr, &dst_size);
            if (res == (size_t)-1)
            {
                if (errno == E2BIG)
                {
                    // ignore this error
                }
                else if (ignore_error_)
                {
                    // skip character
                    ++src_ptr;
                    --src_size;
                }
                else
                {
                    check_convert_error();
                }
            }
            dst.append(&buf[0], buf.size() - dst_size);
        }
        dst.swap(output);
    }

private:
    void check_convert_error() const
    {
        switch (errno)
        {
        case EILSEQ:
            throw std::runtime_error("invalid multibyte chars");
        case EINVAL:
            throw std::runtime_error("invalid multibyte chars");
        default:
            throw std::runtime_error("unknown error");
        }
    }

    iconv_t iconv_;
    bool ignore_error_;
    const size_t buf_size_;
};
#endif // ICONVPP_H_INCLUDED
