#include <clutter/clutter.h>
#include "gmc-carousel.h"
#include "gmc-list-view.h"
#include "gmc-video-controler.h"
#include "gmc-button.h"

void
clicked_cb (GmcButton *button)
{
  g_debug ("%s called by %s", G_STRFUNC, gmc_button_get_text (button));
}

int
main (int argc, char **argv)
{
  ClutterActor *stage, *list_view, *background;
  ClutterScript *script;
  ClutterModel *list_model;
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
  //clutter_stage_set_fullscreen (CLUTTER_STAGE (stage), TRUE);

  background = CLUTTER_ACTOR (clutter_script_get_object (script, "background"));
  clutter_actor_set_size (background, CLUTTER_STAGE_WIDTH (), CLUTTER_STAGE_HEIGHT ());
  /*list_model = clutter_list_model_new (2,
                                       G_TYPE_INT,      "Score",
                                       G_TYPE_STRING,   "Team");

  clutter_model_append (list_model, 0, 1, 1, "Team a", -1);
  clutter_model_append (list_model, 0, 2, 1, "Team b", -1);*/

  //list_view = gmc_list_view_new (list_model, 1);
  //clutter_actor_set_size (list_view, 100, 100);

  clutter_actor_show_all (stage);
  
  clutter_main ();

  return 0;
}
