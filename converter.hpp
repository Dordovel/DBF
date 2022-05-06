#include <string>
#include <iconv.h>
#include <cstring>

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
