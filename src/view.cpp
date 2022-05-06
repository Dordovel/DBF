//
// Created by user on 5/6/22.
//

#include <memory>
#include <langinfo.h>
#include "../headers/view.hpp"
#include "../converter.hpp"

View::View(Gtk::Window::BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& m_RefGlade) : Gtk::Window(cobject)
{
	m_RefGlade->get_widget("Table", this->_view);
	this->encoding = nl_langinfo(CODESET);
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

	auto record = this->_treeModel->append();

	for(decltype (fields)::size_type i = 0; i < fields.size(); ++i)
	{
		this->_view->append_column(fields[i], this->_columns[i]);
	}
}

void View::view_record(std::vector<std::string> record)
{
	auto refRecord = *(this->_treeModel->append());
	for(decltype(this->_columns)::size_type i = 0; i < this->_columns.size(); ++i)
	{
		refRecord[this->_columns[i]] = convert_encoding(record[i], "CP866", encoding);
	}
}
