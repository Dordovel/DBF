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

	const int rowCounts = dbf_file.get_row_count();
	for (int i = 0; i < rowCounts; ++i)
	{
		auto record = dbf_file.get_dbf_records(i);
		std::cout<<"{"<<std::endl;
		for(const auto& column : record)
		{
			std::string new_value = convert_encoding(column.second, "CP866", encoding);
			std::cout<<"\t"<<column.first << "  "<<new_value<<std::endl;
		}
		std::cout<<"}"<<std::endl;
	}

	return EXIT_SUCCESS;
}
