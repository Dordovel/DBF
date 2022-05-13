#include "./headers/dbf.hpp"
#include <langinfo.h>
#include "headers/view.hpp"
#include "gtkmm/cssprovider.h"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");

	std::string filePath;

	if(argc > 1)
	{
		filePath = argv[1];
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
		view->set_size_request(1500, 800);
		view->convert_from("CP866");
		view->load(&dbf_file);
	}

	auto css = Gtk::CssProvider::create();
	if(!css->load_from_path("./style/general.css"))
	{
		return EXIT_FAILURE;
	}
	auto screen = Gdk::Screen::get_default();
	auto ctx = view->get_style_context();
	ctx->add_provider_for_screen(screen, css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	app->run(*view);
	return EXIT_SUCCESS;
}
