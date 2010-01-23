#include <clutter/clutter.h>

int
main (int argc, char **argv)
{
  ClutterActor *stage;
  ClutterColor stage_color = { 0x34, 0x39, 0x00, 0xff };
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

  clutter_actor_show_all (stage);
  
  clutter_main ();

  return 0;
}
