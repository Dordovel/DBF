//
// Created by user on 5/6/22.
//

#include <cmath>
#include <iostream>
#include <memory>
#include <langinfo.h>
#include "gtkmm/button.h"
#include "gtkmm/enums.h"
#include "sigc++/functors/mem_fun.h"

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
	m_RefGlade->get_widget("Panel", this->_box);
	m_RefGlade->get_widget("Filter", this->_filter);
	m_RefGlade->get_widget("ClosePanel", this->_closePanelBtn);
	m_RefGlade->get_widget("PrevPage", this->_dataLoadPrevPageButton);
	m_RefGlade->get_widget("Pages", this->_dataLoadPageLabel);
	m_RefGlade->get_widget("NextPage", this->_dataLoadNextPageButton);
	m_RefGlade->get_widget("LoadBox", this->_dataLoadContainer);

	this->_dataLoadPrevPageButton->signal_clicked().connect(sigc::mem_fun(this, &View::signal_data_load_prev_page_button));
	this->_dataLoadNextPageButton->signal_clicked().connect(sigc::mem_fun(this, &View::signal_data_load_next_page_button));
	this->_currentPage = 1;
	this->_elementPerPage = 200;

	this->_filter->signal_changed().connect(sigc::mem_fun(this, &View::signal_entry_change));
	this->_record.add(this->_id);
	this->_view->append_column("ID", this->_id);
}

void View::view_header(std::vector<Field> fields)
{
	this->_columns.resize(fields.size());
	for(auto& column : this->_columns)
	{
		this->_record.add(column);
	}

	this->_treeModel = Gtk::ListStore::create(this->_record);
	this->_filterModel = Gtk::TreeModelFilter::create(this->_treeModel);
	this->_filterModel->set_visible_func(sigc::mem_fun(this, &View::signal_row_visible));
	this->_view->set_model(this->_filterModel);

	for(decltype (fields)::size_type i = 0; i < fields.size(); ++i)
	{
		this->_view->append_column(std::string(fields[i].Name) + ", " + fields[i].Type,
									this->_columns[i]);
	}
}

void View::view_record(std::vector<std::string> record)
{
	auto refRecord = *(this->_treeModel->append());

	const std::size_t id = this->_treeModel->children().size();
	refRecord[this->_id] = id;

	for(decltype(this->_columns)::size_type i = 0; i < this->_columns.size(); ++i)
	{
		Field&& field = this->_dbf_file->get_field_info(i);
		refRecord[this->_columns[i]] = this->parse_value(this->encode_value(record[i]), field.Type);
	}
}

void View::load(DBF* dbf)
{
	this->_dbf_file = dbf;

	if(this->_view)
	{
		std::vector<Field> fields;

		const std::size_t fieldsCount = this->_dbf_file->get_fields_count();
		for(int i = 0; i < fieldsCount; ++i)
		{
			Field&& info = this->_dbf_file->get_field_info(i);
			fields.emplace_back(info);
		}

		this->view_header(std::move(fields));
		this->update_page_label();
		this->read_page();

		this->_view->signal_row_activated().connect(sigc::mem_fun(this,&View::signal_edit));
	}

	if(this->_box)
	{
		if(this->_closePanelBtn)
		{
			this->_closePanelBtn->signal_clicked().connect(sigc::mem_fun(this, &View::view_dbf_header_panel));
		}
		this->view_dbf_header_panel();
	}
}

void View::view_dbf_header_panel()
{
	Header header = this->_dbf_file->get_header_info();

	Gtk::Box* Tag = Gtk::manage(new Gtk::Box());
	Gtk::Label* TagHeader = Gtk::manage(new Gtk::Label());
	TagHeader->set_label("?????????????? ?????????? ???????????????????? (DBT): ");

	Gtk::Label* TagState = Gtk::manage(new Gtk::Label());
	if(header.Tag == 0x83)
	{
		TagState->set_label("????????");
	}
	else if(header.Tag == 0x03)
	{
		TagState->set_label("??????");
	}

	Tag->pack_start(*TagHeader, false, false, 0);
	Tag->pack_start(*TagState, false, false, 0);

	Gtk::Box* Records = Gtk::manage(new Gtk::Box());
	Gtk::Label* RecordHeader = Gtk::manage(new Gtk::Label());
	RecordHeader->set_label("???????????????????? ??????????????: ");

	Gtk::Label* RecordCount = Gtk::manage(new Gtk::Label());
	RecordCount->set_label(std::to_string(header.FileSize));

	Records->pack_start(*RecordHeader, false, false, 0);
	Records->pack_start(*RecordCount, false, false, 0);

	Gtk::Box* LoadRecords = Gtk::manage(new Gtk::Box());
	Gtk::Label* LoadRecordHeader = Gtk::manage(new Gtk::Label());
	LoadRecordHeader->set_label("???????????????????? ?????????????????????? ??????????????: ");

	Gtk::Label* LoadRecordCount = Gtk::manage(new Gtk::Label());
	LoadRecordCount->set_label(std::to_string(this->_treeModel->children().size()));

	LoadRecords->pack_start(*LoadRecordHeader, false, false, 0);
	LoadRecords->pack_start(*LoadRecordCount, false, false, 0);

	Gtk::Box* Date = Gtk::manage(new Gtk::Box());
	Gtk::Label* DateHeader = Gtk::manage(new Gtk::Label());
	DateHeader->set_label("???????? ??????????????????: ");

	Gtk::Label* DateLabel = Gtk::manage(new Gtk::Label());

	int year = header.LastUpdate[0];
	int month = header.LastUpdate[1];
	int day = header.LastUpdate[2];

	DateLabel->set_label(std::to_string(day) + "-" + std::to_string(month) + "-" + std::to_string(year));

	Date->pack_start(*DateHeader, false, false, 0);
	Date->pack_start(*DateLabel, false, false, 0);

	for(auto* child : this->_box->get_children())
	{
		this->_box->remove(*child);
	}

	this->_box->pack_start(*Tag, false, false, 0);
	this->_box->pack_start(*Records, false, false, 0);
	this->_box->pack_start(*LoadRecords, false, false, 0);
	this->_box->pack_start(*Date, false, false, 0);
	this->_box->set_size_request(300, 300);
	this->_box->show_all_children();
}

void View:: view_record_edit_panel(unsigned long id)
{
	const std::size_t recordId = id - 1;
	const std::size_t dbfRecordId = this->convert_page_to_dbf_record(recordId);
	const auto&& record = this->_dbf_file->get_record(dbfRecordId);
	
	for(auto* child : this->_box->get_children())
	{
		this->_box->remove(*child);
	}

	std::vector<Gtk::Entry*> entry;
	entry.reserve(record.size());
	
	for(int i = 0; i < record.size(); ++i)
	{
		const auto&& fieldInfo = this->_dbf_file->get_field_info(i);

		Gtk::Label* field = Gtk::manage(new Gtk::Label());
		field->set_size_request(150);
		field->set_alignment(Gtk::ALIGN_START);
		field->set_label(std::string(fieldInfo.Name) + " (" + std::to_string(fieldInfo.Width) + ")");

		Gtk::Entry* value = Gtk::manage(new Gtk::Entry());
		value->set_max_length(fieldInfo.Width);
		value->set_text(this->encode_value(record[i]));

		Gtk::Box* box = Gtk::manage(new Gtk::Box());
		box->pack_start(*field, false, false, 0);
		box->pack_start(*value, true, true, 0);

		this->_box->pack_start(*box, false, false, 0);

		entry.push_back(value);
	}

	Gtk::Button* saveBtn = Gtk::manage(new Gtk::Button());
	saveBtn->set_label("??????????????????");
	saveBtn->signal_clicked().connect(
			[
				this,
				entries = std::move(entry),
				dbfRecordId
			]
			()
			{
				this->signal_save_record(dbfRecordId, entries);
			});

	Gtk::Button* deleteBtn = Gtk::manage(new Gtk::Button());
	deleteBtn->set_label("??????????????");
	deleteBtn->signal_clicked().connect([this, recordId](){this->signal_delete_record(recordId);});

	Gtk::Box* box = Gtk::manage(new Gtk::Box());
	box->pack_start(*saveBtn, true, true, 5);
	box->pack_start(*deleteBtn, false, false, 0);
		
	this->_box->pack_start(*box, false, false, 0);
	this->_box->show_all_children();
}

std::string View::encode_value(std::string value)
{
	if(!this->_fileEncoding.empty())
	{
		return convert_encoding(rtrim(std::move(value)), this->_fileEncoding, this->_systemEncoding);
	}

	return value;
}

std::string View::parse_value(std::string date, char type)
{
	switch (type)
	{
		case 'D':
			return this->parse_date(std::move(date));
		default:
			return date;
	}
}

std::string View::parse_date(std::string date)
{
	if(date.empty()) return "";

	std::string year = date.substr(0, 4);
	std::string month = date.substr(4, 2);
	std::string day = date.substr(6, 2);

	return day + "-" + month + "-" + year;
}

void View::signal_edit(const Gtk::TreePath& path, Gtk::TreeViewColumn* column)
{
	auto selection = this->_view->get_selection();
	auto selectedIter = selection->get_selected();
	auto value = selectedIter->get_value(this->_id);

	this->view_record_edit_panel(value);
}

bool View::signal_row_visible (const Gtk::TreeModel::const_iterator& iter)
{
	if(this->_filter)
	{
		std::string text = this->_filter->get_text();
		if(!text.empty())
		{
			for(const auto& column : this->_columns)
			{
				if(iter->get_value(column).find(text) != std::string::npos)
				{
					return true;
				}
			}

			return false;
		}
	}
	return true;
}

void View::signal_entry_change()
{
	this->_filterModel->refilter();
}

void View::signal_delete_record(std::size_t recordId)
{
	this->_dbf_file->delete_record(recordId);
}

void View::signal_save_record(std::size_t recordId, std::vector<Gtk::Entry*> entries)
{
	if(entries.empty()) return;

	std::vector<std::string> dbf_record;
	dbf_record.reserve(entries.size());

	auto children = this->_treeModel->children();
	auto child = children[recordId];

	for( decltype(this->_columns)::size_type i = 0; i < this->_columns.size(); ++i)
	{
		std::string new_value = entries[i]->get_text();
		std::string converted_value = convert_encoding(new_value, this->_systemEncoding, this->_fileEncoding);

		Field&& field = this->_dbf_file->get_field_info(i);
		child[this->_columns[i]] = this->parse_value(new_value, field.Type);
		dbf_record.emplace_back(converted_value);
	}

	const std::size_t dbfRecordId = this->convert_page_to_dbf_record(recordId);
	this->_dbf_file->replace_record(dbfRecordId, dbf_record);
}

void View::update_page_label()
{
	const int pageCount = std::ceil(this->_dbf_file->get_header_info().FileSize / static_cast<float>(this->_elementPerPage));
	this->_dataLoadPageLabel->set_label(
		std::to_string(this->_currentPage)
		+ "/"
		+ std::to_string(pageCount));
}

void View::read_page()
{
	this->_treeModel->clear();
	std::size_t elementPerPage = this->_elementPerPage;

	float allPages = this->_dbf_file->get_header_info().FileSize / static_cast<float>(this->_elementPerPage);
	std::size_t fullPages = static_cast<int>(allPages);
	std::size_t elementsOnFullPages = fullPages * this->_elementPerPage;
	std::size_t tailPageElementCount = this->_dbf_file->get_header_info().FileSize - elementsOnFullPages;
	std::size_t tailPage = std::ceil(allPages);

	if(tailPage > 0)
	{
		if(this->_currentPage == tailPage)
		{
			elementPerPage = tailPageElementCount;
		}
	}

	for ( std::size_t i = 0; i < elementPerPage; ++i )
	{
		auto record = this->_dbf_file->get_next_record();
		this->view_record(std::move(record));
	}
}

std::size_t View::convert_page_to_dbf_record(std::size_t viewRecordId)
{
	return (this->_currentPage - 1) * this->_elementPerPage + viewRecordId;
}

void View::signal_data_load_next_page_button()
{
	const int pageCount = this->_dbf_file->get_header_info().FileSize / this->_elementPerPage;
	if(this->_currentPage < pageCount)
	{
		const std::size_t recordPointer = ( this->_currentPage * this->_elementPerPage);
		this->_dbf_file->move_to_record(recordPointer);

		this->_currentPage++;
		this->update_page_label();
		this->read_page();
	}
}

void View::signal_data_load_prev_page_button()
{
	if(this->_currentPage > 1)
	{
		const std::size_t recordPointer = ( this->_currentPage * this->_elementPerPage ) - (this->_elementPerPage * 2);
		this->_dbf_file->move_to_record(recordPointer);

		this->_currentPage--;
		this->update_page_label();
		this->read_page();
	}
}

void View::convert_from(std::string fileEncoding)
{
	this->_fileEncoding = std::move(fileEncoding);
	this->_systemEncoding = nl_langinfo(CODESET);
}
