#include "./headers/dbf.hpp"
#include "./converter.hpp"
#include <langinfo.h>

int main()
{
	setlocale(LC_ALL, "");
	char* encoding = nl_langinfo(CODESET);
	DBF dbf_file;
	dbf_file.loadDbf("./1.DBF");
	//dbf_file.add_record();
	//int count = dbf_file.get_record_count();
	//dbf_file.replace_record(count - 1, "TABNUM", "qwe");
	dbf_file.insert_record(16);
	return EXIT_SUCCESS;
}
