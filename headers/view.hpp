#ifndef VIEW
#define VIEW

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

		char* encoding = nullptr;

	public:
		View(Gtk::Window::BaseObjectType* cobject,
			 const Glib::RefPtr<Gtk::Builder>& m_RefGlade);
		void view_header(std::vector<std::string> fields);
		void view_record(std::vector<std::string> record);
};
#endif // VIEW