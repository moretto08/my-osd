/*
 * Copyright (C) 2026 Mateus
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */

#include <gtkmm.h>
#include <gtk-layer-shell.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <filesystem>

namespace fs = std::filesystem;
const char* SOCKET_PATH = "/tmp/my_osd.sock";

class OSD : public Gtk::Window {
private:
    Gtk::LevelBar* bar = nullptr;
    Gtk::Label* label = nullptr;
    sigc::connection timeout_conn;

public:
    OSD(int value, std::string icon_name, std::string text) {
        gtk_layer_init_for_window(gobj());
        gtk_layer_set_namespace(gobj(), "my-osd");
        gtk_layer_set_layer(gobj(), GTK_LAYER_SHELL_LAYER_OVERLAY);
        gtk_layer_set_anchor(gobj(), GTK_LAYER_SHELL_EDGE_BOTTOM, true);
        gtk_layer_set_margin(gobj(), GTK_LAYER_SHELL_EDGE_BOTTOM, 100);

        set_name("osd-window");

        auto* main_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_VERTICAL, 5);
        main_box->set_name("osd-main-container");

        auto* content_box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, 15);
        content_box->set_name("osd-box");
        content_box->set_halign(Gtk::ALIGN_CENTER);
        
        auto* icon = Gtk::make_managed<Gtk::Image>();
        icon->set_from_icon_name(icon_name, Gtk::ICON_SIZE_DIALOG);
        content_box->pack_start(*icon, Gtk::PACK_SHRINK);

        if (value >= 0) {
            bar = Gtk::make_managed<Gtk::LevelBar>();
            bar->set_value(value / 100.0);
            bar->set_size_request(200, 10);
            bar->set_valign(Gtk::ALIGN_CENTER);
            content_box->pack_start(*bar, Gtk::PACK_SHRINK);
        }

        main_box->pack_start(*content_box, Gtk::PACK_SHRINK);

        if (!text.empty()) {
            label = Gtk::make_managed<Gtk::Label>(text);
            label->set_name("osd-label");
            main_box->pack_start(*label, Gtk::PACK_SHRINK);
        }
        
        add(*main_box);
        show_all_children();
        reset_timer();
    }

    void update_osd(int value, std::string text) {
    if (bar) {
        if (value >= 0) {
            bar->set_value(value / 100.0);
            bar->show();
        } else {
            bar->hide(); 
        }
    }
    
    if (label) {
        label->set_markup("");
        label->set_text(text);
        label->show();
    }
    
    while (Gtk::Main::events_pending()) Gtk::Main::iteration();
    
    reset_timer();
}

    void reset_timer() {
        if (timeout_conn.connected()) timeout_conn.disconnect();
        timeout_conn = Glib::signal_timeout().connect([this]() {
            Gtk::Main::quit();
            return false;
        }, 1500);
    }
};

struct OSDData {
    int value;
    char text[32];
};

bool on_socket_data(Glib::IOCondition cond, int server_fd, OSD* osd) {
    if (cond & Glib::IO_IN) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd >= 0) {
            OSDData data;
            memset(&data, 0, sizeof(OSDData));
            
            if (read(client_fd, &data, sizeof(OSDData)) > 0) {
                osd->update_osd(data.value, std::string(data.text));
            }
            close(client_fd);
        }
    }
    return true;
}
int main(int argc, char* argv[]) {
    int value = (argc > 1) ? std::stoi(argv[1]) : 0;
    std::string icon = (argc > 2) ? argv[2] : "audio-volume-high";
    std::string text = (argc > 3) ? argv[3] : "";
    
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1);

    int sock = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
        OSDData data;
        data.value = value;
        strncpy(data.text, text.c_str(), sizeof(data.text)-1);
        send(sock, &data, sizeof(data), 0);
        close(sock);
        return 0; 
    }
    close(sock);

    unlink(SOCKET_PATH);
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) return 1;
    listen(server_fd, 10);
    chmod(SOCKET_PATH, 0666);

    Gtk::Main kit(argc, argv);

auto css_provider = Gtk::CssProvider::create();
    std::string home_config = std::string(std::getenv("HOME")) + "/.config/my-osd/style.css";
    
    fs::path exe_dir = fs::absolute(argv[0]).parent_path();
    fs::path local_css = exe_dir / "style.css";
    
    std::string shared_css = "/usr/local/share/my-osd/style.css";

    try {
        if (fs::exists(local_css)) {
            css_provider->load_from_path(local_css.string());
        } else if (fs::exists(home_config)) {
            css_provider->load_from_path(home_config);
        } else if (fs::exists(shared_css)) {
            css_provider->load_from_path(shared_css);
        }

        Gtk::StyleContext::add_provider_for_screen(
            Gdk::Screen::get_default(), 
            css_provider, 
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
        );
    } catch (...) {
    }

    Gtk::StyleContext::add_provider_for_screen(Gdk::Screen::get_default(), css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    OSD osd(value, icon, text);
    Glib::signal_io().connect(sigc::bind(sigc::ptr_fun(&on_socket_data), server_fd, &osd), server_fd, Glib::IO_IN);

    osd.show_all();
    Gtk::Main::run(osd);
    unlink(SOCKET_PATH);
    return 0;
}
