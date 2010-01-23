#include <clutter/clutter.h>
#include <clutter-gst/clutter-gst.h>

G_BEGIN_DECLS

#define GMC_TYPE_VIDEO_CONTROLER            (gmc_video_controler_get_type ())
#define GMC_VIDEO_CONTROLER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GMC_TYPE_VIDEO_CONTROLER, GmcVideoControler))
#define GMC_VIDEO_CONTROLER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GMC_TYPE_VIDEO_CONTROLER, GmcVideoControlerClass))
#define GMC_IS_VIDEO_CONTROLER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMC_TYPE_VIDEO_CONTROLER))
#define GMC_IS_VIDEO_CONTROLER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GMC_TYPE_VIDEO_CONTROLER))
#define GMC_VIDEO_CONTROLER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GMC_TYPE_VIDEO_CONTROLER, GmcVideoControlerClass))

typedef struct _GmcVideoControler        GmcVideoControler;
typedef struct _GmcVideoControlerPrivate GmcVideoControlerPrivate;
typedef struct _GmcVideoControlerClass   GmcVideoControlerClass;

struct _GmcVideoControler
{
  ClutterGroup              parent;
  /*< priv >*/
  GmcVideoControlerPrivate *priv;
};

struct _GmcVideoControlerClass 
{
  ClutterGroupClass parent_class;
}; 

GType           gmc_video_controler_get_type           (void) G_GNUC_CONST;

ClutterActor *  gmc_video_controler_new                (ClutterMedia *media);

G_END_DECLS
