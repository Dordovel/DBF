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

	if(view != nullptr)
	{
		view->set_size_request(700, 500);
		view->convert_from("CP866");
		view->load(std::move(dbf_file));
	}

	app->run(*view);
	return EXIT_SUCCESS;
}
