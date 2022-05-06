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
		const int Offset = 32 + sizeof(DBF::_Field_) * i;
		this->_file.seekg(Offset, std::ios::beg);
		this->_file.read((char*)field.Name, sizeof(DBF::_Field_::Name));
		this->_file.read((char*)&field.Type, sizeof(DBF::_Field_::Type));
		this->_file.read((char*)&field.Offset, sizeof(DBF::_Field_::Offset));
		this->_file.read((char*)&field.Width, sizeof(DBF::_Field_::Width));
		this->_file.read((char*)&field.Precision, sizeof(DBF::_Field_::Precision));
		this->_file.read((char*)field.Other, sizeof(DBF::_Field_::Other));

		field.Offset = Offset;

		this->_fields.push_back(field);
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

void DBF::flushHeaderChange()
{
	this->_file.seekg(std::ios::beg);
	this->_file.write((char*)&this->_header.Tag, sizeof(DBF::_Header_::Tag));
	this->_file.write((char*)&this->_header.LastUpdate, sizeof(DBF::_Header_::LastUpdate));
	this->_file.write((char*)&this->_header.FileSize, sizeof(DBF::_Header_::FileSize));
	this->_file.write((char*)&this->_header.HeaderSize, sizeof(DBF::_Header_::HeaderSize));
	this->_file.write((char*)&this->_header.RecordSize, sizeof(DBF::_Header_::RecordSize));
	this->_file.write((char*)&this->_header.Other, sizeof(DBF::_Header_::Other));
}

void DBF::end()
{
	const char EOF_record = 0x1A;
	const int RecordSize = this->_header.RecordSize;
	const int LastRecordTail = this->_header.HeaderSize + (RecordSize * this->_header.FileSize);

	this->_file.seekg(LastRecordTail + RecordSize, std::ios::beg);
	this->_file.write(&EOF_record, sizeof (EOF_record));
}

void DBF::loadDbf(const std::string& filePath)
{
	this->_file.open(filePath, std::ios::binary | std::ios::in | std::ios::out);
	if(this->_file.is_open())
	{
		this->loadDbfTableStructure();
		this->loadDbfTableFields();
	}
}

DBF::~DBF()
{
	if(this->_file.is_open())
	{
		this->_file.close();
	}
}

std::unordered_map<std::string, std::string> DBF::get_record_with_names(int record)
{
	std::unordered_map<std::string, std::string> result;

	int position = this->_header.HeaderSize + (this->_header.RecordSize * record) + 1;
	this->_file.seekg(position, std::ios::beg);

	for(const DBF::_Field_& field : this->_fields)
	{
		char buffer[field.Width];
		memset(buffer, '\0', field.Width);
		this->_file.read(buffer, field.Width);

		result.emplace(field.Name, std::string(buffer, field.Width));
	}
	return result;
}

std::vector<std::string> DBF::get_record(int record)
{
	std::vector <std::string> result;

	int position = this->_header.HeaderSize + (this->_header.RecordSize * record) + 1;
	this->_file.seekg(position, std::ios::beg);

	for(const DBF::_Field_& field : this->_fields)
	{
		char buffer[field.Width];
		memset(buffer, '\0', field.Width);

		this->_file.read(buffer, field.Width);

		result.emplace_back(std::string(buffer, field.Width));
	}

	return result;
}

void DBF::rename_field(int field, std::string_view newName)
{
	if(field >= 0 && field < this->_fields.size())
	{
		const auto& replaced = this->_fields.at(field);

		const std::size_t FieldWidth = std::size(replaced.Name);

		if(newName.size() <= FieldWidth)
		{
			this->_file.seekg(replaced.Offset, std::ios::beg);
			this->_file.write(newName.data(), FieldWidth);
		}
	}
}

int DBF::get_fields_count() const
{
	return this->_fields.size();
}

int DBF::get_record_count() const
{
	return this->_header.FileSize;
}

void DBF::replace_record(int record, std::vector<std::string> new_record)
{
	if(this->_header.FileSize > record)
	{
		if(new_record.size() <= this->_fields.size())
		{
			int position = this->_header.HeaderSize + ( this->_header.RecordSize * record);
			this->_file.seekg(position, std::ios::beg);
			for(decltype(this->_fields)::size_type i = 0; i < new_record.size(); ++i)
			{
				const DBF::_Field_& field = this->_fields.at(i);
				std::string new_value = new_record.at(i);
				this->_file.write((char*)new_value.c_str(), field.Width);
			}
		}
	}
}

void DBF::replace_record(int record, int column, std::string_view new_record)
{
	if(this->_header.FileSize > record)
	{
		if(new_record.size() <= this->_fields.size())
		{
			if(column < this->_fields.size())
			{
				const int position = this->_header.HeaderSize + ( this->_header.RecordSize * record) + sizeof (char);
				int Offset = position;
				for( decltype(this->_fields)::size_type i = 0; i < column; ++i)
				{
					Offset += this->_fields.at(i).Width;
				}
				this->_file.seekg(Offset, std::ios::beg);

				const DBF::_Field_& field = this->_fields.at(column);
				char buffer[field.Width];
				memset(buffer, '\0', field.Width);
				memcpy(buffer, new_record.data(), new_record.size());

				this->_file.write(buffer, field.Width);
			}
		}
	}
}

void DBF::replace_record(int record, std::string_view column, std::string_view new_record)
{
	if(this->_header.FileSize > record)
	{
		const int position = this->_header.HeaderSize + ( this->_header.RecordSize * record) + sizeof (char);
		int Offset = position;

		DBF::_Field_* field = nullptr;

		for(DBF::_Field_& f : this->_fields)
		{
			if(f.Name == column)
			{
				field = &f;
				break;
			}

			Offset += f.Width;
		}

		if(field != nullptr)
		{
			this->_file.seekg(Offset, std::ios::beg);

			char buffer[field->Width];
			memset(buffer, '\0', field->Width);
			memcpy(buffer, new_record.data(), new_record.size());

			this->_file.write(buffer, field->Width);
		}
	}
}

void DBF::add_record()
{
	int lastRecordTail = this->_header.HeaderSize + (this->_header.RecordSize * this->_header.FileSize);

	const int RecordSize = this->_header.RecordSize;

	char buffer[RecordSize];
	memset(buffer, ' ', RecordSize);

	this->_file.seekg(lastRecordTail, std::ios::beg);
	this->_file.write(buffer, RecordSize);
	this->end();

	this->_header.FileSize = this->_header.FileSize + 1;
	this->flushHeaderChange();
}

void DBF::insert_record(int record)
{
	const int RecordSize = this->_header.RecordSize;
	const int RecordCount = this->_header.FileSize;
	int lastRecordTail = this->_header.HeaderSize + (RecordSize * RecordCount);

	char buffer[RecordSize];
	memset(buffer, ' ', RecordSize);

	for(int i = RecordCount; i > record; --i)
	{
		this->_file.seekg(lastRecordTail - RecordSize, std::ios::beg);
		this->_file.read(buffer, RecordSize);

		this->_file.seekg(lastRecordTail, std::ios::beg);
		this->_file.write(buffer, RecordSize);

		lastRecordTail = lastRecordTail - RecordSize;
	}
	memset(buffer, ' ', RecordSize);
	this->_file.write(buffer, RecordSize);

	this->end();

	this->_header.FileSize = this->_header.FileSize + 1;
	this->flushHeaderChange();
}

void DBF::delete_record(int record)
{
	int position = this->_header.HeaderSize + (this->_header.RecordSize * record);
	this->_file.seekg(position, std::ios::beg);
	char mark = '*';
	this->_file.write(&mark, sizeof(mark));
}

Header DBF::get_header_info() const
{
	Header new_header;
	memcpy(&new_header, &this->_header, sizeof (Header));

	return new_header;
}

Field DBF::get_field_info(int field) const
{
	Field new_field;

	if(field >= 0 && field < this->_fields.size())
	{
		DBF::_Field_ copy_field = this->_fields.at(field);
		memcpy(&new_field, &copy_field, sizeof (Field));
	}

	return new_field;
}

void DBF::pack()
{

}
