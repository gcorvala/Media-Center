#include <stdlib.h>
#include <clutter/clutter.h>
#include <clutter-gst/clutter-gst.h>
#include <clutter-gtk/clutter-gtk.h>
#include <cairo/cairo.h>

#include "gmc-video-controler.h"

ClutterActor *stage, *video, *video_controler, *cover;
GtkWidget *window, *embed;

void
resize_cb 	(GtkWidget     *widget,
			 GtkAllocation *allocation,
			 gpointer       user_data)
{
	g_debug ("resize_cb");

	gfloat w, h, ratio;

	clutter_actor_set_size (stage, allocation->width, allocation->height);

	clutter_actor_get_size (video, &w, &h);
	ratio = allocation->width / w;
	clutter_actor_set_size (video, allocation->width, h * ratio);
	clutter_actor_set_position (video, 0, (allocation->height - h) / 2);

	clutter_actor_get_size (video_controler, &w, &h);
	clutter_actor_set_position (video_controler, 0, allocation->height - h);
}

gboolean
key_cb (ClutterActor *actor,
	    ClutterEvent *event,
		gpointer      user_data)
{
	g_debug ("key_cb");

	if (clutter_event_get_key_symbol (event) == 'f')
		gtk_window_maximize (GTK_WINDOW (window));
	else if (clutter_event_get_key_symbol (event) == 'g')
		gtk_window_unmaximize (GTK_WINDOW (window));
}

main (int argc, char **argv)
{
	ClutterActor *mirror;
	GtkBuilder *builder;
	GstElement *elem;
	GError *error = NULL;

	clutter_init (&argc, &argv);
	clutter_gst_init (&argc, &argv);
	gtk_init (&argc, &argv);

	builder = gtk_builder_new ();
	if (!gtk_builder_add_from_file (builder, "window.ui", &error)) {
		g_critical ("gtk_builder : %d - %s", error->code, error->message);
	}

	window = GTK_WIDGET (gtk_builder_get_object (builder, "window"));
	embed = GTK_WIDGET (gtk_builder_get_object (builder, "embed"));

	g_signal_connect (embed, "size-allocate", G_CALLBACK (resize_cb), NULL);

	stage = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (embed));
	clutter_stage_set_title (CLUTTER_STAGE (stage), "Hello World");
	clutter_stage_set_user_resizable (CLUTTER_STAGE (stage), FALSE);
	clutter_stage_set_color (CLUTTER_STAGE (stage), clutter_color_new (255, 0, 0, 255));
	clutter_stage_set_key_focus (CLUTTER_STAGE (stage), NULL);
	clutter_actor_set_reactive (stage, TRUE);

	video = clutter_gst_video_texture_new ();
	clutter_actor_set_reactive (video, TRUE);
	clutter_media_set_filename (CLUTTER_MEDIA (video), "../data/Fringe.1x17.avi");

	cover = clutter_texture_new_from_file ("../data/avatar.jpg", &error);
	if (!cover)
		g_critical ("cover : %d - %s", error->code, error->message);
	clutter_actor_set_height (cover, 200);

	mirror = clutter_clone_new (cover);
	clutter_actor_set_rotation (mirror, CLUTTER_X_AXIS, 180, 0, 240, 0);
	clutter_actor_set_y (mirror, clutter_actor_get_height (cover));
	clutter_actor_set_opacity (mirror, 100);

	video_controler = gmc_video_controler_new (CLUTTER_MEDIA (video));

	clutter_container_add (CLUTTER_CONTAINER (stage),
						   video,
						   cover,
						   mirror,
						   video_controler,
						   NULL);

	clutter_actor_show (stage);

	gtk_widget_show_all (window);

	g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (clutter_main_quit), NULL);
	g_signal_connect (stage, "key-press-event", G_CALLBACK (key_cb), NULL);
	clutter_main ();

	return EXIT_SUCCESS;
}