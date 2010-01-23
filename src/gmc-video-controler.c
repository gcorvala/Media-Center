#include "gmc-video-controler.h"

G_DEFINE_TYPE (GmcVideoControler, gmc_video_controler, CLUTTER_TYPE_GROUP);

#define GMC_VIDEO_CONTROLER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GMC_TYPE_VIDEO_CONTROLER, GmcVideoControlerPrivate))

struct _GmcVideoControlerPrivate
{
  ClutterTimeline *activity;
  ClutterMedia *media;
  ClutterActor *play;
  ClutterActor *pause;
  ClutterActor *stop;
  ClutterActor *duration;
  ClutterActor *progress;
};

enum {
  PROP_0,
  PROP_MEDIA
};

static void
gmc_video_controler_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  GmcVideoControler *controler;
  GmcVideoControlerPrivate *priv;

  controler = GMC_VIDEO_CONTROLER (object);
  priv = GMC_VIDEO_CONTROLER_GET_PRIVATE (controler);

  switch (prop_id)
  {
    case PROP_MEDIA:
      priv->media = g_value_get_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gmc_video_controler_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  GmcVideoControler *controler;
  GmcVideoControlerPrivate *priv;

  controler = GMC_VIDEO_CONTROLER (object);
  priv = GMC_VIDEO_CONTROLER_GET_PRIVATE (controler);
  switch (prop_id)
  {
    case PROP_MEDIA:
      g_value_set_object (value, priv->media);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gmc_video_controler_class_init (GmcVideoControlerClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (gobject_class, sizeof (GmcVideoControlerPrivate));

  gobject_class->set_property = gmc_video_controler_set_property;
  gobject_class->get_property = gmc_video_controler_get_property;

  g_object_class_install_property (gobject_class,
                                   PROP_MEDIA,
                                   g_param_spec_object ("media",
                                                        "Media",
                                                        "The ClutterMedia controlled",
                                                        G_TYPE_OBJECT,
                                                        G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT));
}

gboolean
play_cb (ClutterActor *actor,
         ClutterEvent *event,
         gpointer      user_data)
{
  g_debug ("play_cb");

  GmcVideoControler *self;
  GmcVideoControlerPrivate *priv;
  gdouble duration;

  self = GMC_VIDEO_CONTROLER (user_data);
  priv = GMC_VIDEO_CONTROLER_GET_PRIVATE (self);

  clutter_media_set_playing (priv->media, TRUE);
  duration = clutter_media_get_duration (priv->media);

  g_debug ("%f", duration);

  return TRUE;
}

gboolean
pause_cb (ClutterActor *actor,
          ClutterEvent *event,
          gpointer      user_data)
{
  g_debug ("pause_cb");

  GmcVideoControler *self;
  GmcVideoControlerPrivate *priv;

  self = GMC_VIDEO_CONTROLER (user_data);
  priv = GMC_VIDEO_CONTROLER_GET_PRIVATE (self);

  if (!clutter_media_get_playing (priv->media))
    clutter_media_set_progress (priv->media, clutter_media_get_progress (priv->media) + 0.01);
  else
    clutter_media_set_playing (priv->media, FALSE);

  return TRUE;
}

gboolean
stop_cb (ClutterActor *actor,
         ClutterEvent *event,
         gpointer      user_data)
{
  g_debug ("stop_cb");

  GmcVideoControler *self;
  GmcVideoControlerPrivate *priv;

  self = GMC_VIDEO_CONTROLER (user_data);
  priv = GMC_VIDEO_CONTROLER_GET_PRIVATE (self);

  clutter_media_set_playing (priv->media, FALSE);
  clutter_media_set_progress (priv->media, 0.0);

  return TRUE;
}

gboolean
activity_cb (ClutterActor *actor,
             ClutterEvent *event,
             gpointer      user_data)
{
  g_debug ("activity_cb");

  GmcVideoControler *self;
  GmcVideoControlerPrivate *priv;

  self = GMC_VIDEO_CONTROLER (user_data);
  priv = GMC_VIDEO_CONTROLER_GET_PRIVATE (self);

  clutter_actor_set_opacity (CLUTTER_ACTOR (self), 255);
  clutter_timeline_rewind (priv->activity);
  clutter_timeline_start (priv->activity);

  return TRUE;
}

void
no_activity_cb (ClutterTimeline *timeline,
                gpointer         user_data)
{
  g_debug ("no_activity_cb");

  GmcVideoControler *self;
  GmcVideoControlerPrivate *priv;

  self = GMC_VIDEO_CONTROLER (user_data);
  priv = GMC_VIDEO_CONTROLER_GET_PRIVATE (self);

  clutter_actor_animate (CLUTTER_ACTOR (self), CLUTTER_LINEAR, 1000,
                         "opacity", 0,
                         NULL);
}

void
change_cb (ClutterActor *actor,
           gpointer      user_data)
{
  g_debug ("change_cb");

  GmcVideoControler *self;
  GmcVideoControlerPrivate *priv;
  gdouble progress, duration;
  gchar *duration_text;

  self = GMC_VIDEO_CONTROLER (user_data);
  priv = GMC_VIDEO_CONTROLER_GET_PRIVATE (self);

  progress = clutter_media_get_progress (priv->media);
  duration = clutter_media_get_duration (priv->media);

  duration_text = g_strdup_printf ("%d / %d", duration * progress, duration);
  clutter_text_set_text (CLUTTER_TEXT (priv->duration), duration_text);
}

static void
gmc_video_controler_init (GmcVideoControler *self)
{
  GmcVideoControlerPrivate *priv;
  ClutterColor *color;
  GError *error = NULL;

  self->priv = priv = GMC_VIDEO_CONTROLER_GET_PRIVATE (self);

  priv->play = clutter_texture_new_from_file ("../img/play.png", &error);
  if (!priv->play)
    g_critical ("1");
  
  priv->pause = clutter_texture_new_from_file ("../img/pause.png", &error);
  if (!priv->pause)
    g_critical ("2");

  priv->stop = clutter_texture_new_from_file ("../img/stop.png", &error);
  if (!priv->stop)
    g_critical ("3");

  priv->duration = clutter_text_new ();
  clutter_text_set_text (CLUTTER_TEXT (priv->duration), "aBc");

  priv->progress = clutter_rectangle_new ();
  color = clutter_color_new (0, 0, 0, 255);
  clutter_rectangle_set_color (CLUTTER_RECTANGLE (priv->progress), color);
  color = clutter_color_new (255, 255, 255, 255);
  clutter_rectangle_set_border_color (CLUTTER_RECTANGLE (priv->progress), color);
  clutter_rectangle_set_border_width (CLUTTER_RECTANGLE (priv->progress), 2);
  clutter_actor_set_size (priv->progress, 400, 10);

  clutter_actor_set_position (priv->play, 0, 0);
  clutter_actor_set_position (priv->pause, 48, 0);
  clutter_actor_set_position (priv->stop, 96, 0);
//  clutter_actor_set_position (priv->duration, 144, 0);
  clutter_actor_set_position (priv->progress, 144, 19);

  clutter_actor_set_reactive (priv->play, TRUE);
  clutter_actor_set_reactive (priv->pause, TRUE);
  clutter_actor_set_reactive (priv->stop, TRUE);
  clutter_actor_set_reactive (CLUTTER_ACTOR (priv->media), TRUE);

  priv->activity = clutter_timeline_new (5000);

  g_signal_connect (priv->play, "button-press-event", G_CALLBACK (play_cb), self);
  g_signal_connect (priv->pause, "button-press-event", G_CALLBACK (pause_cb), self);
  g_signal_connect (priv->stop, "button-press-event", G_CALLBACK (stop_cb), self);
  g_signal_connect (priv->activity, "completed", G_CALLBACK (no_activity_cb), self);

  clutter_container_add (CLUTTER_CONTAINER (self),
                         priv->play,
                         priv->pause,
                         priv->stop,
                         //priv->duration,
                         priv->progress,
                         NULL);

  clutter_timeline_start (priv->activity);
}

ClutterActor *
gmc_video_controler_new (ClutterMedia *media)
{
  GObject *self;

  self = g_object_new (GMC_TYPE_VIDEO_CONTROLER,
             "media", media,
             NULL);

  g_signal_connect (media, "motion-event", G_CALLBACK (activity_cb), self);

  return CLUTTER_ACTOR (self);
}
