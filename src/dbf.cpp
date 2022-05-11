#include <iostream>
#include <cstring>
#include <algorithm>
#include "../headers/dbf.hpp"

template <typename T, size_t G>
size_t array_size(T(&array)[G])
{
	return G;
}

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
	this->_file.seekg(std::ios::beg);
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

	this->move_to_record(this->_header.FileSize);

	this->_file.seekg(RecordSize, std::ios::cur);
	this->_file.write(&EOF_record, sizeof (EOF_record));
}

void DBF::loadDbf(const std::string& filePath)
{
	this->_file.open(filePath, std::ios::binary | std::ios::in | std::ios::out);
	if(this->_file.is_open())
	{
		this->loadDbfTableStructure();
		this->loadDbfTableFields();
		this->move_to_record(0);
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
	this->move_to_record(record);
	return this->get_record_with_names();
}

std::vector<std::string> DBF::get_record(int record)
{
	this->move_to_record(record);
	return this->get_record();
}

void DBF::move_to_record(int record)
{
	this->_file.seekg(this->_header.HeaderSize + (this->_header.RecordSize * record), std::ios::beg);
}

void DBF::skip_delete_mark()
{
	const int deleteMark = sizeof(char);
	this->_file.seekg(deleteMark, std::ios::cur);
}

std::unordered_map<std::string, std::string> DBF::get_record_with_names()
{
	std::unordered_map<std::string, std::string> result;

	this->skip_delete_mark();

	for(const DBF::_Field_& field : this->_fields)
	{
		char buffer[field.Width];
		memset(buffer, '\0', field.Width);

		std::streamsize size = this->_file.read(buffer, field.Width).gcount();

		result.emplace(field.Name, std::string(buffer, size));
	}
	return result;
}

std::vector<std::string> DBF::get_record()
{
	std::vector <std::string> result;
	result.reserve(this->_fields.size());

	this->skip_delete_mark();

	for(const DBF::_Field_& field : this->_fields)
	{
		char buffer[field.Width];
		memset(buffer, '\0', field.Width);

		std::streamsize size = this->_file.read(buffer, field.Width).gcount();

		std::string value = std::string(buffer,field.Width);
		result.push_back(std::move(value));
	}

	return result;
}

void DBF::rename_field(int field, std::string newName)
{
	if(field >= 0 && field < this->_fields.size())
	{
		const auto& replaced = this->_fields.at(field);

		const std::size_t FieldWidth = array_size(replaced.Name);

		if(newName.size() <= FieldWidth)
		{
			int lastPoistion = this->_file.tellg();
			this->_file.seekg(replaced.Offset, std::ios::beg);
			this->_file.write(newName.data(), FieldWidth);
			this->_file.seekg(lastPoistion, std::ios::beg);
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
			this->move_to_record(record);
			this->skip_delete_mark();

			for(decltype(this->_fields)::size_type i = 0; i < this->_fields.size(); ++i)
			{
				const DBF::_Field_& field = this->_fields[i];
				std::string new_value = std::move(new_record[i]);

				char buffer[field.Width];
				memset(buffer, '\0', field.Width);
				memcpy(buffer, new_value.data(), new_value.size());

				this->_file.write(buffer, field.Width);
			}
		}
	}
}

void DBF::replace_record(int record, int column, std::string new_record)
{
	if(this->_header.FileSize > record)
	{
		if(new_record.size() <= this->_fields.size())
		{
			if(column < this->_fields.size())
			{
				this->move_to_record(record);
				this->skip_delete_mark();

				int Offset = 0 ;

				for( decltype(this->_fields)::size_type i = 0; i < column; ++i)
				{
					Offset += this->_fields[i].Width;
				}
				this->_file.seekg(Offset, std::ios::cur);

				const DBF::_Field_& field = this->_fields[column];
				char buffer[field.Width];
				memset(buffer, '\0', field.Width);
				memcpy(buffer, new_record.data(), new_record.size());

				this->_file.write(buffer, field.Width);
			}
		}
	}
}

void DBF::replace_record(int record, std::string column, std::string new_record)
{
	if(this->_header.FileSize > record)
	{
		this->move_to_record(record);
		this->skip_delete_mark();

		int Offset = 0;

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
			this->_file.seekg(Offset, std::ios::cur);

			char buffer[field->Width];
			memset(buffer, '\0', field->Width);
			memcpy(buffer, new_record.data(), new_record.size());

			this->_file.write(buffer, field->Width);
		}
	}
}

void DBF::add_record()
{
	const int deleteMark = sizeof(char);
	const int RecordSize = this->_header.RecordSize + deleteMark;

	char buffer[RecordSize];
	memset(buffer, ' ', RecordSize);

	this->move_to_record(this->_header.FileSize);
	this->_file.write(buffer, RecordSize);
	this->end();

	this->_header.FileSize = this->_header.FileSize + 1;
	this->flushHeaderChange();
}

void DBF::insert_record(int record)
{
	const int deleteMark = sizeof(char);
	const int RecordSize = this->_header.RecordSize + deleteMark;
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
	this->move_to_record(record);
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
