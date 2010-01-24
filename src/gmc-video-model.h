#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GMC_TYPE_VIDEO_MODEL            (gmc_video_model_get_type ())
#define GMC_VIDEO_MODEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GMC_TYPE_VIDEO_MODEL, GmcVideoModel))
#define GMC_VIDEO_MODEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GMC_TYPE_VIDEO_MODEL, GmcVideoModelClass))
#define GMC_IS_VIDEO_MODEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMC_TYPE_VIDEO_MODEL))
#define GMC_IS_VIDEO_MODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GMC_TYPE_VIDEO_MODEL))
#define GMC_VIDEO_MODEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GMC_TYPE_VIDEO_MODEL, GmcVideoModelClass))

typedef struct _GmcVideoModel        GmcVideoModel;
typedef struct _GmcVideoModelPrivate GmcVideoModelPrivate;
typedef struct _GmcVideoModelClass   GmcVideoModelClass;

struct _GmcVideoModel
{
  ClutterModel              parent;
  /*< priv >*/
  GmcVideoModelPrivate *priv;
};

struct _GmcVideoModelClass 
{
  ClutterModelClass parent_class;
}; 

GType           gmc_video_model_get_type           (void) G_GNUC_CONST;

ClutterModel *  gmc_video_model_new                ();

G_END_DECLS
