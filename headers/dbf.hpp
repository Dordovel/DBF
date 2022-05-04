#ifndef DBF_FILE
#define DBF_FILE

#include <vector>
#include <unordered_map>
#include <fstream>

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

		struct _Record_
		{
			int RecordOffset;
			std::string Value;
		};

		struct _Row_
		{
			std::unordered_map<std::string, DBF::_Record_> Rows;
			char Mark;
		};

		struct _Header_ _header;
		std::vector<struct _Field_> _fields;
		std::vector<DBF::_Row_> _rows;


		std::fstream _file;

		void loadDbfTableFields();
		void loadDbfRecords();
		void loadDbfTableStructure();

		void dbf_write(const DBF::_Field_& field, int rowId, std::string_view value);

	public:
		void loadDbf(const std::string& filePath);
		std::unordered_map<std::string, std::string> get_dbf_records(int row);
		int get_row_count() const;
		void dbf_write(int columnId, int rowId, std::string_view value);
		void dbf_write(std::string_view columnName, int rowId, std::string_view value);
		DBF() = default;
		~DBF();

};

#endif //DBF_FILE
