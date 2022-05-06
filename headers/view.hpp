#ifndef VIEW
#define VIEW

#include "dbf.hpp"
#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>

class View : public Gtk::Window
{
	private:
		Gtk::TreeView* _view = nullptr;
		Glib::RefPtr<Gtk::ListStore> _treeModel;

		Gtk::TreeModel::ColumnRecord _record;
		std::vector<Gtk::TreeModelColumn<std::string>> _columns;
		Gtk::TreeModelColumn<std::size_t> _id;

		DBF _dbf_file;
		char* _systemEncoding = nullptr;
		std::string _fileEncoding;

		void view_header(std::vector<std::string> fields);
		void view_record(std::vector<std::string> record);

	public:
		View(Gtk::Window::BaseObjectType* cobject,
			 const Glib::RefPtr<Gtk::Builder>& m_RefGlade);

		void convert_from(std::string fileEncoding);
		void load(DBF&& dbf);
};
#endif // VIEW
