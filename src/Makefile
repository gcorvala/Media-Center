CC = gcc -g

CLUTTER_LIBS = `pkg-config --libs clutter-1.0`
CLUTTER_CFLAGS = `pkg-config --cflags clutter-1.0`
CLUTTER_GST_LIBS = `pkg-config --libs clutter-gst-0.10`
CLUTTER_GST_CFLAGS = `pkg-config --cflags clutter-gst-0.10`
COGL_LIBS = `pkg-config --libs cogl-1.0`
COGL_CFLAGS = `pkg-config --cflags cogl-1.0`
GIO_LIBS = `pkg-config --libs gio-2.0`
GIO_CFLAGS = `pkg-config --cflags gio-2.0`
SQLITE_LIBS = `pkg-config --libs sqlite3`
SQLITE_CFLAGS = `pkg-config --cflags sqlite3`

OBJECTS = main.o \
		  gmc-carousel.o \
		  gmc-list-view.o \
		  gmc-video-controler.o \
		  gmc-video-model.o

all: main

main: $(OBJECTS)
	$(CC) $(CLUTTER_LIBS) $(CLUTTER_GST_LIBS) $(COGL_LIBS) $(GIO_LIBS) $(SQLITE_LIBS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CLUTTER_CFLAGS) $(CLUTTER_GST_CFLAGS) -c -o $@ $<

clean:
	rm $(OBJECTS) main
