#include <cctype>
#include <string>
#include <iconv.h>
#include <exception>
#include <string.h>
#include <vector>

std::string convert_encoding_old(const std::string& data, const std::string& from, const std::string& to)
{
   if (data.empty())
   {
      return std::string();
   }
   iconv_t convert_hnd = iconv_open(to.c_str(), from.c_str());
   if (convert_hnd == (iconv_t)(-1))
   {
      throw std::logic_error("unable to create convertion descriptor");
   }
 
   char* in_ptr = const_cast<char*>(data.c_str());
   std::size_t in_size = data.size();
   std::vector<char> outbuf(6 * data.size());
   char* out_ptr = &outbuf[0];
   std::size_t reverse_size = outbuf.size();
 
   std::size_t result = iconv(convert_hnd, &in_ptr, &in_size, &out_ptr, &reverse_size);
   iconv_close(convert_hnd);
   if (result == (std::size_t)(-1))
   {
      throw std::logic_error("unable to convert");
   }
   return std::string(outbuf.data(), outbuf.size() - reverse_size);
}

std::string convert_encoding(const std::string& src, const char* from, const char* to)
{
	iconv_t convertHnd = iconv_open(to, from);
	if (convertHnd == (iconv_t)(-1))
	{
	  throw std::logic_error("unable to create convertion descriptor");
	}

	size_t size = src.size();

	size_t bufferSize = 6 * size;
	char buffer[bufferSize];

	char* buffer_ptr = buffer;
	memset(buffer, '\0', bufferSize);

	char src_copy[size];
	strncpy(src_copy, src.c_str(), size);

	char* src_copy_ptr = src_copy;

	size_t res = iconv(convertHnd, &src_copy_ptr, &size, &buffer_ptr, &bufferSize);

	std::string result = std::string(buffer, strlen(buffer));

	iconv_close(convertHnd);

	return result;
}
