#ifndef DBF_FILE
#define DBF_FILE

#include <vector>
#include <unordered_map>
#include <fstream>

struct Header
{
	char Tag;
	char LastUpdate[3];
	int FileSize;
	short HeaderSize;
	short RecordSize;
	char Other[20];
};

struct Field
{
	char Name[11];
	char Type;
	int Offset;
	unsigned char Width;
	char Precision;
	char Other[14];
};

class DBF
{
	private:
		struct _Field_
		{
			char Name[11];
			char Type;
			int Offset;
			unsigned char Width;
			char Precision;
			char Other[14];
		};

		struct _Header_
		{
			char Tag;
			char LastUpdate[3];
			int FileSize;
			short HeaderSize;
			short RecordSize;
			char Other[20];
		};

		struct _Header_ _header;
		std::vector<struct DBF::_Field_> _fields;

		std::fstream _file;

		void loadDbfTableFields();
		void loadDbfTableStructure();

	public:
		void loadDbf(const std::string& filePath);
		std::unordered_map<std::string, std::string> get_record_with_names(int record);
		std::vector<std::string> get_record(int record);
		void replace_record(int record, std::vector<std::string> new_record);
		void replace_record(int record, int column, std::string_view new_record);
		void replace_record(int record, std::string_view column, std::string_view new_record);
		void rename_field(int field, std::string_view newName);
		int get_fields_count() const;
		int get_record_count() const;
		void add_record();
		void delete_record(int record);
		Header get_header_info() const;
		Field get_field_info(int field) const;
		void pack();

		DBF() = default;
		~DBF();

};

#endif //DBF_FILE
