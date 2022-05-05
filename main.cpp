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
	dbf_file.get_record(15);
	return EXIT_SUCCESS;
}
