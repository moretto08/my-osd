#include <gtkmm.h>
#include <gtk-layer-shell.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

const char* SOCKET_PATH = "/tmp/my_osd.sock";

class OSD : public Gtk::Window {
private:
    Gtk::LevelBar* bar;
    sigc::connection timeout_conn;

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
            if (visual) gtk_widget_set_visual(GTK_WIDGET(gobj()), visual->gobj());
        }

        set_name("osd-window");

        auto* box = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL, 15);
        box->set_name("osd-box");
        box->set_halign(Gtk::ALIGN_CENTER);
        
        auto* icon = Gtk::make_managed<Gtk::Image>();
        icon->set_from_icon_name(icon_name, Gtk::ICON_SIZE_DIALOG);

        bar = Gtk::make_managed<Gtk::LevelBar>();
        bar->set_value(value / 100.0);
        bar->set_size_request(200, 10);
        bar->set_valign(Gtk::ALIGN_CENTER);

        box->pack_start(*icon, Gtk::PACK_SHRINK);
        box->pack_start(*bar, Gtk::PACK_SHRINK);
        
        add(*box);
        show_all_children();
        reset_timer();
    }

    void update_osd(int value) {
        bar->set_value(value / 100.0);
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

bool on_socket_data(Glib::IOCondition cond, int server_fd, OSD* osd) {
    if (cond & Glib::IO_IN) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd >= 0) {
            char buffer[32];
            int n = read(client_fd, buffer, sizeof(buffer)-1);
            if (n > 0) {
                buffer[n] = '\0';
                try {
                    int val = std::stoi(buffer);
                    osd->update_osd(val);
                } catch (...) {}
            }
            close(client_fd);
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    int value = (argc > 1) ? std::stoi(argv[1]) : 0;
    
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1);

    // Tenta conectar como cliente de forma não-bloqueante e ultra rápida
    int sock = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0 || errno == EINPROGRESS) {
        // Pequeno delay para garantir que o socket está pronto se estiver em progresso
        fd_set fdset;
        struct timeval tv = {0, 10000}; // 10ms timeout
        FD_ZERO(&fdset);
        FD_SET(sock, &fdset);
        
        if (select(sock + 1, NULL, &fdset, NULL, &tv) > 0) {
            std::string val_str = std::to_string(value);
            send(sock, val_str.c_str(), val_str.length(), 0);
            close(sock);
            return 0; // Missão cumprida, fecha a instância secundária
        }
    }
    close(sock);

    // Se falhou a conexão, esta vira a instância principal (Servidor)
    unlink(SOCKET_PATH);
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) return 1;
    listen(server_fd, 10);
    chmod(SOCKET_PATH, 0666);

    Gtk::Main kit(argc, argv);

    auto css_provider = Gtk::CssProvider::create();
    try {
        css_provider->load_from_path("/usr/local/share/my-osd/style.css");
        Gtk::StyleContext::add_provider_for_screen(Gdk::Screen::get_default(), css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    } catch (...) {}

    OSD osd(value, (argc > 2) ? argv[2] : "audio-volume-high");
    Glib::signal_io().connect(sigc::bind(sigc::ptr_fun(&on_socket_data), server_fd, &osd), server_fd, Glib::IO_IN);

    osd.show_all();
    Gtk::Main::run(osd);

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}