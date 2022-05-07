//
// Created by user on 5/6/22.
//

#include <memory>
#include <langinfo.h>
#include "../headers/view.hpp"
#include "../headers/converter.hpp"

static inline std::string rtrim(std::string s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());

	return s;
}

View::View(Gtk::Window::BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& m_RefGlade) : Gtk::Window(cobject)
{
	m_RefGlade->get_widget("Table", this->_view);
	this->_systemEncoding = nl_langinfo(CODESET);

	this->_record.add(this->_id);
	this->_view->append_column("ID", this->_id);
}

void View::view_header(std::vector<std::string> fields)
{
	this->_columns.resize(fields.size());
	for(auto& column : this->_columns)
	{
		this->_record.add(column);
	}

	this->_treeModel = Gtk::ListStore::create(this->_record);
	this->_view->set_model(this->_treeModel);

	for(decltype (fields)::size_type i = 0; i < fields.size(); ++i)
	{
		this->_view->append_column(fields[i], this->_columns[i]);
	}
}

void View::view_record(std::vector<std::string> record)
{
	auto refRecord = *(this->_treeModel->append());

	const std::size_t id = this->_treeModel->children().size();
	refRecord[this->_id] = id;

	for(decltype(this->_columns)::size_type i = 0; i < this->_columns.size(); ++i)
	{
		refRecord[this->_columns[i]] = convert_encoding(rtrim(record[i]), this->_fileEncoding, this->_systemEncoding);
	}
}

void View::load(DBF&& dbf)
{
	this->_dbf_file = std::move(dbf);

	std::vector<std::string> fields;
	const std::size_t fieldsCount = this->_dbf_file.get_fields_count();
	for(int i = 0; i < fieldsCount; ++i)
	{
		Field&& info = this->_dbf_file.get_field_info(i);
		fields.emplace_back(info.Name);
	}

	this->view_header(std::move(fields));

	const std::size_t recordsCount = this->_dbf_file.get_record_count();
	for(int i = 0; i < recordsCount; ++i)
	{
		auto&& record = this->_dbf_file.get_record(i);
		this->view_record(std::move(record));
	}

}

void View::convert_from(std::string fileEncoding)
{
	this->_fileEncoding = std::move(fileEncoding);
}
