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
		Glib::Dispatcher _dataLoadDispatcher;
		Gtk::Button* _dataLoadCancelButton;
		Gtk::Box* _dataLoadContainer;

		std::thread _dataLoadThread;
		std::unique_ptr<std::vector<std::vector<std::string>>> _dataLoadBuffer = nullptr;
		std::mutex _dataLoadMutex;
		std::condition_variable _dataLoadCV;
		std::atomic_bool _dataLoadStatus;
		typename decltype(View::_dataLoadBuffer)::element_type::size_type _dataLoadBufferSize;

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
		void view_record_edit_panel(unsigned long id);
		std::string encode_value(std::string value);
		std::string parse_value(std::string date, char type);
		std::string parse_date(std::string date);

		void signal_edit(const Gtk::TreePath& path, Gtk::TreeViewColumn* column);
		bool signal_row_visible (const Gtk::TreeModel::const_iterator& iter);
		void signal_entry_change();
		void signal_delete_record(int recordId);
		void signal_save_record(unsigned long recordId, std::vector<Gtk::Entry*> entries);
		void signal_data_load_cancel_button();

		void data_load_worker();
		void data_update_worker();

	public:
		View(Gtk::Window::BaseObjectType* cobject,
			 const Glib::RefPtr<Gtk::Builder>& m_RefGlade);

		~View();
		void convert_from(std::string fileEncoding);
		void load(DBF* dbf);
};
#endif // VIEW
