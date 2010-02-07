#include <clutter/clutter.h>
#include "gmc-button.h"
#include "gmc-cover-flow-layout.h"
#include "gmc-video-model.h"

void
clicked_cb (GmcButton *button)
{
  g_debug ("%s called by %s", G_STRFUNC, gmc_button_get_label (button));
}

int
main (int argc, char **argv)
{
  ClutterActor *stage, *background, *box, *rectangle;
  ClutterColor *color;
  ClutterScript *script;
  ClutterLayoutManager *cover_flow;
  GError *error = NULL;
  guint id;

  clutter_init (&argc, &argv);

  script = clutter_script_new ();
//  id = clutter_script_load_from_file (script, PACKAGE_DATADIR "/main.ui", &error);
  id = clutter_script_load_from_file (script, "data/main.ui", &error);
  if (!id) {
    g_critical ("clutter_script_load_from_file : %d - %s", error->code, error->message);
    g_object_unref (script);

    return 1;
  }

  clutter_script_connect_signals (script, NULL);

  stage = CLUTTER_ACTOR (clutter_script_get_object (script, "stage"));

  background = CLUTTER_ACTOR (clutter_script_get_object (script, "background"));
  clutter_actor_set_size (background, CLUTTER_STAGE_WIDTH (), CLUTTER_STAGE_HEIGHT ());

  cover_flow = gmc_cover_flow_layout_new ();
  //cover_flow = clutter_flow_layout_new (CLUTTER_FLOW_HORIZONTAL);

  color = clutter_color_new (0x00, 0x00, 0x00, 0xff);
  box = clutter_box_new (cover_flow);
  clutter_box_set_color (CLUTTER_BOX (box), color);

  color = clutter_color_new (0xff, 0x00, 0x00, 0xff);

  rectangle = clutter_rectangle_new_with_color (color);
  //clutter_actor_set_rotation (rectangle, CLUTTER_Y_AXIS, 30, 0, 0, 0);

  clutter_container_add_actor (CLUTTER_CONTAINER (box), rectangle);

  color = clutter_color_new (0x00, 0xff, 0x00, 0xff);

  rectangle = clutter_rectangle_new_with_color (color);
  //clutter_actor_set_rotation (rectangle, CLUTTER_Y_AXIS, 30, 0, 0, 0);

  clutter_container_add_actor (CLUTTER_CONTAINER (box), rectangle);

  clutter_container_add_actor (CLUTTER_CONTAINER (stage), box);

  clutter_actor_show_all (stage);
  
  clutter_main ();

  return 0;
}
