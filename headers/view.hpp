#ifndef VIEW
#define VIEW

#include "dbf.hpp"
#include "gtkmm/button.h"
#include "gtkmm/entry.h"
#include "gtkmm/treemodel.h"
#include "gtkmm/treepath.h"
#include <array>
#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treemodelfilter.h>
#include <gtkmm/box.h>
#include <gtkmm/treestore.h>
#include <glibmm/dispatcher.h>
#include <thread>
#include <condition_variable>
#include <atomic>

class View : public Gtk::Window
{
	private:
		Gtk::Box* _box = nullptr;
		Gtk::Entry* _filter = nullptr;
		Gtk::Button* _closePanelBtn;
		Gtk::Button* _dataLoadNextPageButton;
		Gtk::Button* _dataLoadPrevPageButton;
		Gtk::Label* _dataLoadPageLabel;
		Gtk::Box* _dataLoadContainer;
		std::size_t _currentPage;
		std::size_t _elementPerPage;

		Gtk::TreeView* _view = nullptr;
		Glib::RefPtr<Gtk::ListStore> _treeModel;
		Glib::RefPtr<Gtk::TreeModelFilter> _filterModel;

		Gtk::TreeModel::ColumnRecord _record;
		std::vector<Gtk::TreeModelColumn<std::string>> _columns;
		Gtk::TreeModelColumn<std::size_t> _id;

		DBF* _dbf_file;
		char* _systemEncoding = nullptr;
		std::string _fileEncoding;

		void view_header(std::vector<Field> fields);
		void view_record(std::vector<std::string> record);
		void view_dbf_header_panel();
		void view_record_edit_panel(std::size_t id);
		std::string encode_value(std::string value);
		std::string parse_value(std::string date, char type);
		std::string parse_date(std::string date);

		void signal_edit(const Gtk::TreePath& path, Gtk::TreeViewColumn* column);
		bool signal_row_visible (const Gtk::TreeModel::const_iterator& iter);
		void signal_entry_change();
		void signal_delete_record(std::size_t recordId);
		void signal_save_record(std::size_t recordId, std::vector<Gtk::Entry*> entries);
		void signal_data_load_next_page_button();
		void signal_data_load_prev_page_button();

		void update_page_label();
		void read_page();
		std::size_t convert_page_to_dbf_record(std::size_t viewRecordId);

	public:
		View(Gtk::Window::BaseObjectType* cobject,
			 const Glib::RefPtr<Gtk::Builder>& m_RefGlade);

		~View() = default;
		void convert_from(std::string fileEncoding);
		void load(DBF* dbf);
};
#endif // VIEW
