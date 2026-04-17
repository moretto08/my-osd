#include <gtkmm.h>
#include <gtkmm/main.h>
#include <gtk-layer-shell.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>

class OSD : public Gtk::Window {
public:
    OSD(int value, std::string icon_name) {
        gtk_layer_init_for_window(gobj());
	    gtk_layer_set_namespace(gobj(), "my-osd");
        gtk_layer_set_layer(gobj(), GTK_LAYER_SHELL_LAYER_OVERLAY);
        gtk_layer_set_anchor(gobj(), GTK_LAYER_SHELL_EDGE_BOTTOM, true);
        gtk_layer_set_margin(gobj(), GTK_LAYER_SHELL_EDGE_BOTTOM, 100);

        auto screen = get_screen();
        if (screen) {
            auto visual = screen->get_rgba_visual();
            if (visual) {
                gtk_widget_set_visual(GTK_WIDGET(gobj()), visual->gobj());
            }
        }

        set_app_paintable(true);
        set_decorated(false);
        set_name("osd-window");

        auto* box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, 15);
        box->set_name("osd-box"); // ID que usaremos no CSS
        box->set_margin_start(15);
        box->set_margin_end(15);
        box->set_margin_top(10);
        box->set_margin_bottom(10);
        
        auto* icon = Gtk::make_managed<Gtk::Image>();
        icon->set_from_icon_name(icon_name, Gtk::ICON_SIZE_DIALOG);
        
        auto* bar = Gtk::make_managed<Gtk::LevelBar>();
        bar->set_value(value / 100.0);
        bar->set_hexpand(true);
        bar->set_valign(Gtk::ALIGN_CENTER);
        bar->set_size_request(200, 12); // Tamanho mínimo garantido

        box->pack_start(*icon, Gtk::PACK_SHRINK);
        box->pack_start(*bar, Gtk::PACK_EXPAND_WIDGET);
        
        add(*box);
        show_all_children();
        
        Glib::signal_timeout().connect([this]() {
            Gtk::Main::quit();
            return false;
        }, 2000);
    }
};

int main(int argc, char* argv[]) {
    Gtk::Main kit(argc, argv);

    std::string style_path;
    const char* homeDir = std::getenv("HOME");

    std::string userConfig = homeDir ? + "/.config/my-osd/style.css" : "";
    std::string systemConfig = "/usr/local/share/my-osd/style.css";

    if (std::filesystem::exists(userConfig)) {
	style_path = userConfig;
    } else if (std::filesystem::exists(systemConfig)) {
 	style_path = systemConfig;
    } else {
	style_path = "style.css";
    }

    auto css_provider = Gtk::CssProvider::create();
    try {
        css_provider->load_from_path(style_path);
        Gtk::StyleContext::add_provider_for_screen(
            Gdk::Screen::get_default(), css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
    } catch (const Gtk::CssProviderError& ex) {
        std::cerr << "Erro ao carregar CSS: " << ex.what() << std::endl;
    } catch (const Glib::Error& ex) {
        std::cerr << "Erro inesperado ao carregar estilo: " << ex.what() << std::endl;
    }

    Gtk::StyleContext::add_provider_for_screen(
        Gdk::Screen::get_default(), css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    int value = (argc > 1) ? std::stoi(argv[1]) : 0;
    std::string icon = (argc > 2) ? argv[2] : "audio-volume-high";

    OSD osd(value, icon);
    osd.show_all();

    Gtk::Main::run(osd);
    return 0;
}
