#include <clutter/clutter.h>
#include "gmc-button.h"
#include "gmc-video-model.h"

ClutterModel *model;

void
clicked_cb (GmcButton *button)
{
  g_debug ("%s called by %s", G_STRFUNC, gmc_button_get_label (button));
  clutter_model_append (model, 0, gmc_button_get_label (button), 1, 2007, -1);
}

int
main (int argc, char **argv)
{
  ClutterActor *stage, *background;
  ClutterScript *script;
  ClutterModelIter *iter;
  GError *error = NULL;
  GValue value = {0, };
  guint id;
  gint n, i;

  clutter_init (&argc, &argv);

  script = clutter_script_new ();
  id = clutter_script_load_from_file (script, PACKAGE_DATADIR "/main.ui", &error);
  if (!id) {
    g_critical ("clutter_script_load_from_file : %d - %s", error->code, error->message);
    g_object_unref (script);

    return 1;
  }

  clutter_script_connect_signals (script, NULL);

  stage = CLUTTER_ACTOR (clutter_script_get_object (script, "stage"));

  background = CLUTTER_ACTOR (clutter_script_get_object (script, "background"));
  clutter_actor_set_size (background, CLUTTER_STAGE_WIDTH (), CLUTTER_STAGE_HEIGHT ());

  model = gmc_video_model_new ("movie.db");
  g_debug ("n_rows : %d", clutter_model_get_n_rows (model));
  n = clutter_model_get_n_columns (model);
  g_debug ("n_columns : %d", n);
  for (i = 0; i < n; ++i)
    g_debug ("column %d name : %s", i, clutter_model_get_column_name (model, i));
  clutter_model_set_sorting_column (model, 0);
  for (i = 0, iter = clutter_model_get_first_iter (model); i < 100 && iter != NULL; ++i, iter = clutter_model_iter_next (iter)) {
    clutter_model_iter_get_value (iter, 0, &value);
    g_print ("Title = %s", g_value_get_string (&value));
    g_value_unset (&value);
    clutter_model_iter_get_value (iter, 1, &value);
    g_print ("\t\tYear = %u \n", g_value_get_uint (&value));
    g_value_unset (&value);
  }

  clutter_actor_show_all (stage);
  
  clutter_main ();

  return 0;
}
