#include <clutter/clutter.h>
#include "gmc-button.h"
#include "gmc-video-model.h"

void
clicked_cb (GmcButton *button)
{
  g_debug ("%s called by %s", G_STRFUNC, gmc_button_get_label (button));
}

int
main (int argc, char **argv)
{
  ClutterActor *stage, *background;
  ClutterScript *script;
  ClutterModel *list_model;
  GError *error = NULL;
  guint id;
  gint n, i;

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

  background = CLUTTER_ACTOR (clutter_script_get_object (script, "background"));
  clutter_actor_set_size (background, CLUTTER_STAGE_WIDTH (), CLUTTER_STAGE_HEIGHT ());

  list_model = gmc_video_model_new ("movie.db");
  g_debug ("n_rows : %d", clutter_model_get_n_rows (list_model));
  n = clutter_model_get_n_columns (list_model);
  g_debug ("n_columns : %d", n);
  for (i = 0; i < n; ++i)
    g_debug ("column %d name : %s", i, clutter_model_get_column_name (list_model, i));
  clutter_model_append (list_model, 0, "hello", 1, 2007, -1);

  clutter_actor_show_all (stage);
  
  clutter_main ();

  return 0;
}
