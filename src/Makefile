CC = gcc

CLUTTER_LIBS = `pkg-config --libs clutter-1.0`
CLUTTER_CFLAGS = `pkg-config --cflags clutter-1.0`
CLUTTER_GST_LIBS = `pkg-config --libs clutter-gst-0.10`
CLUTTER_GST_CFLAGS = `pkg-config --cflags clutter-gst-0.10`
CLUTTER_GTK_LIBS = `pkg-config --libs clutter-gtk-0.10`
CLUTTER_GTK_CFLAGS = `pkg-config --cflags clutter-gtk-0.10`

OBJECTS = gmc-video-controler.o

# gmc-movie-controler.o \ # ClutterActor
# gmc-movie-model.o \ # ClutterModel
# gmc-movie-view.o \ # ClutterActor
# gmc-music-controler.o
# gmc-music-model.o
# gmc-music-view.o

all: main

main: main.c $(OBJECTS)
	$(CC) $(CLUTTER_LIBS) $(CLUTTER_CFLAGS) $(CLUTTER_GST_LIBS) $(CLUTTER_GST_CFLAGS) $(CLUTTER_GTK_LIBS) $(CLUTTER_GTK_CFLAGS) -o $@ $< $(OBJECTS)

%.o: %.c %.h
	$(CC) $(CLUTTER_CFLAGS) $(CLUTTER_GST_CFLAGS) -c -o $@ $<

clean:
	rm $(OBJECTS) main
