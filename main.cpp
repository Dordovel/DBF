#include <iostream>
#include "./headers/dbf.hpp"
#include "./converter.hpp"
#include <langinfo.h>

int main()
{
	setlocale(LC_ALL, "");
	char* encoding = nl_langinfo(CODESET);
	DBF dbf_file;
	dbf_file.loadDbf("./1.DBF");
	std::size_t count = dbf_file.get_record_count();
	for(std::size_t i = 0; i < count; ++i)
	{
		std::vector<std::string> record = dbf_file.get_record(i);
		std::cout<<"{"<<std::endl;
		for(const auto& val : record)
		{
			std::cout<<"  "<<convert_encoding(val, "CP866", encoding)<<std::endl;
		}
		std::cout<<"}"<<std::endl;
	}
	return EXIT_SUCCESS;
}
