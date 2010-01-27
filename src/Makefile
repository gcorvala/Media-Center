CC = gcc -g

ENV_MX = /Users/gabriel/Documents/Media-Center/mx-bin/lib/pkgconfig
ENV_CLUTTER = /Users/gabriel/Documents/Media-Center/clutter-bin/lib/pkgconfig
ENV_CLUTTER_GST = /Users/gabriel/Documents/Media-Center/clutter-gst-bin/lib/pkgconfig

PKG_CONFIG_PATH = $(ENV_CLUTTER):$(ENV_CLUTTER_GST)

CLUTTER_LIBS = `PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs clutter-1.0`
CLUTTER_CFLAGS = `PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags clutter-1.0`
CLUTTER_GST_LIBS = `PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs clutter-gst-0.10`
CLUTTER_GST_CFLAGS = `PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags clutter-gst-0.10`
COGL_LIBS = `pkg-config --libs cogl-1.0`
COGL_CFLAGS = `pkg-config --cflags cogl-1.0`
GIO_LIBS = `pkg-config --libs gio-2.0`
GIO_CFLAGS = `pkg-config --cflags gio-2.0`
SQLITE_LIBS = `pkg-config --libs sqlite3`
SQLITE_CFLAGS = `pkg-config --cflags sqlite3`
MX_LIBS = `$(ENV_MX) pkg-config --libs mx-1.0`
MX_CFLAGS = `$(ENV_MX) pkg-config --cflags mx-1.0`

OBJECTS = main.o gmc-button.o gmc-carousel.o gmc-list-view.o gmc-video-controler.o gmc-video-model.o

all: main

main: $(OBJECTS)
	$(CC) $(CLUTTER_LIBS) $(CLUTTER_GST_LIBS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CLUTTER_CFLAGS) $(CLUTTER_GST_CFLAGS)-c -o $@ $<

clean:
	rm $(OBJECTS) main
