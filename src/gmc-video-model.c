#include "gmc-video-model.h"
#include <sqlite3.h>

G_DEFINE_TYPE (GmcVideoModel, gmc_video_model, CLUTTER_TYPE_MODEL);

#define GMC_VIDEO_MODEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GMC_TYPE_VIDEO_MODEL, GmcVideoModelPrivate))

struct _GmcVideoModelPrivate
{
  gint lol;
};

static void
gmc_video_model_class_init (GmcVideoModelClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (gobject_class, sizeof (GmcVideoModelPrivate));
}

static void
gmc_video_model_init (GmcVideoModel *self)
{
  GmcVideoModelPrivate *priv;
}

ClutterModel *
gmc_video_model_new (void)
{
  GObject *self;

  self = g_object_new (GMC_TYPE_VIDEO_MODEL,
                       NULL);

  return CLUTTER_MODEL (self);
}
