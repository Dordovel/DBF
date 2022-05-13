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
		void flushHeaderChange();
		void end();
		void skip_delete_mark();

	public:
		void loadDbf(const std::string& filePath);
		std::unordered_map<std::string, std::string> get_record_with_names(std::size_t record);
		std::vector<std::string> get_record(std::size_t record);
		std::unordered_map<std::string, std::string> get_next_record_with_names();
		std::vector<std::string> get_next_record();
		void move_to_record(std::size_t record);
		void replace_record(std::size_t record, std::vector<std::string> new_record);
		void replace_record(std::size_t record, std::size_t column, std::string new_record);
		void replace_record(std::size_t record, std::string column, std::string new_record);
		void rename_field(std::size_t field, std::string newName);
		int get_fields_count() const;
		int get_record_count() const;
		void add_record();
		void insert_record(std::size_t record);
		void delete_record(std::size_t record);
		Header get_header_info() const;
		Field get_field_info(std::size_t field) const;
		void pack();

		DBF() = default;
		DBF(DBF&& dbf) = default;
		DBF& operator = (DBF&& dbf) = default;
		~DBF();

};

#endif //DBF_FILE
