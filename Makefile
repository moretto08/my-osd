CXX = g++
CXXFLAGS = `pkg-config --cflags gtkmm-3.0 gtk-layer-shell-0` -std=c++17
LIBS = `pkg-config --libs gtkmm-3.0 gtk-layer-shell-0`
TARGET = build/myosd
SRC = main.cpp

PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
SHAREDIR = $(PREFIX)/share/my-osd

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p build
	$(CXX) $(SRC) $(CXXFLAGS) -o $(TARGET) $(LIBS)

install: all
	install -Dm755 $(TARGET) $(DESTDIR)$(BINDIR)/myosd
	install -Dm644 style.css $(DESTDIR)$(SHAREDIR)/style.css

install-user:
	mkdir -p $(HOME)/.config/my-osd
	cp -n style.css $(HOME)/.config/my-osd/style.css || true
	install -m 755 scripts/volume_osd.sh $(HOME)/.config/my-osd/volume_osd.sh
	install -m 755 scripts/brightness_osd.sh $(HOME)/.config/my-osd/brightness_osd.sh

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/myosd
	rm -rf $(DESTDIR)$(SHAREDIR)

clean:
	rm -rf build
