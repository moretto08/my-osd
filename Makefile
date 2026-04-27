CXX = g++
CXXFLAGS = `pkg-config --cflags gtkmm-3.0 gtk-layer-shell-0` -std=c++17
LIBS = `pkg-config --libs gtkmm-3.0 gtk-layer-shell-0`
TARGET = build/myosd
SRC = main.cpp

PREFIX ?= /usr
BINDIR = $(PREFIX)/bin
SHAREDIR = $(PREFIX)/share/myosd
LIBDIR = $(PREFIX)/lib/myosd

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p build
	$(CXX) $(SRC) $(CXXFLAGS) -o $(TARGET) $(LIBS)

install: all
	install -d $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(SHAREDIR)
	install -d $(DESTDIR)$(LIBDIR)

	install -m755 $(TARGET) $(DESTDIR)$(BINDIR)/myosd
	install -m644 style.css $(DESTDIR)$(SHAREDIR)/style.css

	install -m755 scripts/volume_osd.sh $(DESTDIR)$(LIBDIR)/volume_osd.sh
	install -m755 scripts/brightness_osd.sh $(DESTDIR)$(LIBDIR)/brightness_osd.sh
	install -m755 scripts/lock_osd.sh $(DESTDIR)$(LIBDIR)/lock_osd.sh
	
uninstall:
	rm -f $(DESTDIR)$(BINDIR)/myosd
	rm -rf $(DESTDIR)$(SHAREDIR)

clean:
	rm -rf build
