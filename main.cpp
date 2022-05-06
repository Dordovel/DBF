#include "./headers/dbf.hpp"
#include <langinfo.h>
#include "headers/view.hpp"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");

	std::string filePath;

	for (int i = 0; i < argc; ++i)
	{
		if( strcmp(argv[i], "-f") == 0)
		{
			filePath = argv[ i + 1 ];
		}
	}

	if(filePath.empty()) return EXIT_FAILURE;

	DBF dbf_file;
	dbf_file.loadDbf(filePath);

	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("View");
	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();
	builder->add_from_file("./Main.glade");
	View* view = nullptr;
	builder->get_widget_derived("MainWindow", view);

	std::vector<std::string> fields;
	const std::size_t fieldsCount = dbf_file.get_fields_count();
	for(int i = 0; i < fieldsCount; ++i)
	{
		Field&& info = dbf_file.get_field_info(i);
		fields.emplace_back(info.Name);
	}

	view->view_header(fields);

	const std::size_t recordsCount = dbf_file.get_record_count();
	for(int i = 0; i < recordsCount; ++i)
	{
		auto&& record = dbf_file.get_record(i);
		view->view_record(record);
	}

	app->run(*view);
	return EXIT_SUCCESS;
}
