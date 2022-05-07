#ifndef VIEW
#define VIEW

#include "dbf.hpp"
#include <array>
#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/box.h>

class View : public Gtk::Window
{
	private:
		Gtk::Box* _box = nullptr;

		Gtk::TreeView* _view = nullptr;
		Glib::RefPtr<Gtk::ListStore> _treeModel;

		Gtk::TreeModel::ColumnRecord _record;
		std::vector<Gtk::TreeModelColumn<std::string>> _columns;
		Gtk::TreeModelColumn<std::size_t> _id;

		DBF _dbf_file;
		char* _systemEncoding = nullptr;
		std::string _fileEncoding;

		void view_header(std::vector<Field> fields);
		void view_record(std::vector<std::string> record);
		void view_panel(Header header);
		std::string encode_value(std::string value);
		std::string parse_value(std::string date, char type);
		std::string parse_date(std::string date);

	public:
		View(Gtk::Window::BaseObjectType* cobject,
			 const Glib::RefPtr<Gtk::Builder>& m_RefGlade);

		void convert_from(std::string fileEncoding);
		void load(DBF&& dbf);
};
#endif // VIEW
