#include <iostream>
#include <cstring>
#include <algorithm>
#include "../headers/dbf.hpp"

void DBF::loadDbfTableFields()
{
	//Размер заголовка = 32+32*(число_полей)+2

	int ColumnCount = (this->_header.HeaderSize - 32) / 32;

	for(int i = 0; i < ColumnCount; ++i)
	{
		DBF::_Field_ field;
		this->_file.seekg(32 + sizeof(DBF::_Field_) * i, std::ios::beg);
		this->_file.read((char*)field.Name, sizeof(DBF::_Field_::Name));
		this->_file.read((char*)&field.Type, sizeof(DBF::_Field_::Type));
		this->_file.read((char*)&field.Offset, sizeof(DBF::_Field_::Offset));
		this->_file.read((char*)&field.Width, sizeof(DBF::_Field_::Width));
		this->_file.read((char*)&field.Precision, sizeof(DBF::_Field_::Precision));
		this->_file.read((char*)field.Other, sizeof(DBF::_Field_::Other));
		this->_fields.push_back(field);
	}
}

void DBF::loadDbfRecords()
{
	int Offset = this->_header.HeaderSize;
	this->_file.seekg(Offset, std::ios::beg);

	for(int i = 0; i < this->_header.FileSize; ++i)
	{
		DBF::_Record_ record;
		DBF::_Row_ row;

		this->_file.read((char*)&row.Mark, sizeof(row.Mark));
		Offset += sizeof(row.Mark);

		for(auto field : this->_fields)
		{
			const int FieldWidth = static_cast<int>(field.Width);

			record.Value.resize(FieldWidth);

			this->_file.read((char*)&record.Value[0], FieldWidth);

			record.RecordOffset = Offset;

			Offset += FieldWidth;

			row.Rows.emplace(field.Name, record);
		}

		this->_rows.emplace_back(row);
	}
}

void DBF::loadDbfTableStructure()
{
	this->_file.read((char*)&this->_header.Tag, sizeof(DBF::_Header_::Tag));
	this->_file.read((char*)&this->_header.LastUpdate, sizeof(DBF::_Header_::LastUpdate));
	this->_file.read((char*)&this->_header.FileSize, sizeof(DBF::_Header_::FileSize));
	this->_file.read((char*)&this->_header.HeaderSize, sizeof(DBF::_Header_::HeaderSize));
	this->_file.read((char*)&this->_header.RecordSize, sizeof(DBF::_Header_::RecordSize));
	this->_file.read((char*)&this->_header.Other, sizeof(DBF::_Header_::Other));
}

void DBF::dbf_write(int columnId, int rowId, std::string_view value)
{
	if(columnId < this->_fields.size())
	{
		const DBF::_Field_& field = this->_fields.at(columnId);
		this->dbf_write(field, rowId, value);
	}
}

void DBF::dbf_write(std::string_view columnName, int rowId, std::string_view value)
{
	auto findValue = std::find_if(this->_fields.begin(), this->_fields.end(),
								  [value](const DBF::_Field_& field) {return value == field.Name;});
	if(findValue != this->_fields.end())
	{
		this->dbf_write(*findValue, rowId, value);
	}
}

void DBF::dbf_write(const DBF::_Field_& field, int rowId, std::string_view value)
{
	const int FieldWidth = static_cast<int>(field.Width);

	if(FieldWidth >= value.size())
	{
		if(rowId < this->_rows.size())
		{
			const DBF::_Row_& row = this->_rows.at(rowId);
			DBF::_Record_ record = row.Rows.at(field.Name);

			std::string buffer = std::string(value);
			buffer.resize(field.Width);

			this->_file.seekg(record.RecordOffset, std::ios::beg);
			this->_file.write(buffer.c_str(), FieldWidth);
		}
	}
}

void DBF::loadDbf(const std::string& filePath)
{
	this->_file.open(filePath, std::ios::binary | std::ios::in | std::ios::out);
	if(this->_file.is_open())
	{
		this->loadDbfTableStructure();

		this->loadDbfTableFields();
		this->loadDbfRecords();
	}
}

DBF::~DBF()
{
	if(this->_file.is_open())
	{
		this->_file.close();
	}
}

std::unordered_map<std::string, std::string> DBF::get_dbf_records(int row)
{
	std::unordered_map<std::string, std::string> result;
	if(row < this->_rows.size())
	{
		auto findRow = this->_rows.at(row);
		for(const auto& val : findRow.Rows)
		{
			result.emplace(val.first, val.second.Value);
		}
	}

	return result;
}

int DBF::get_row_count() const
{
	return this->_rows.size();
}


/**std::vector<DBF::_Record_> DBF::get_dbf_records()
{
	return this->_records;
}**/
