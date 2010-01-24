#include <clutter/clutter.h>
#include "gmc-carousel.h"

ClutterActor *carousel;

gboolean
add_box (ClutterActor *actor2,
         ClutterEvent *event,
         gpointer      user_data)
{
  ClutterColor *color;
  ClutterActor *actor;

  color = clutter_color_new (0xff, 0x00, 0x00, 0xff);
  actor = clutter_rectangle_new_with_color (color);
  clutter_actor_set_size (actor, 50, 50);
  clutter_container_add (CLUTTER_CONTAINER (carousel), actor, NULL);
  
  return FALSE;
}

int
main (int argc, char **argv)
{
  ClutterActor *stage;
  ClutterScript *script;
  GError *error = NULL;
  guint id;

  clutter_init (&argc, &argv);

  script = clutter_script_new ();
  id = clutter_script_load_from_file (script, "main.ui", &error);
  if (!id) {
    g_critical ("clutter_script_load_from_file : %d - %s", error->code, error->message);
    g_object_unref (script);

    return 1;
  }

  clutter_script_connect_signals (script, NULL);

  stage = CLUTTER_ACTOR (clutter_script_get_object (script, "stage"));
  carousel = gmc_carousel_new ();
  clutter_actor_set_size (carousel, CLUTTER_STAGE_WIDTH (), CLUTTER_STAGE_HEIGHT ());

  clutter_container_add (CLUTTER_CONTAINER (stage), carousel, NULL);

  clutter_actor_show_all (stage);
  
  clutter_main ();

  return 0;
}
