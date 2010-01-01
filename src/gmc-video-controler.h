#include <clutter/clutter.h>
#include <clutter-gst/clutter-gst.h>

G_BEGIN_DECLS

#define GMC_TYPE_VIDEO_CONTROLER (gmc_video_controler_get_type ())

#define GMC_VIDEO_CONTROLER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  GMC_TYPE_VIDEO_CONTROLER, GMCVideoControler))

#define GMC_VIDEO_CONTROLER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  GMC_TYPE_VIDEO_CONTROLER, GMCVideoControlerClass))

#define GMC_IS_VIDEO_CONTROLER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  GMC_TYPE_VIDEO_CONTROLER))

#define GMC_IS_VIDEO_CONTROLER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  GMC_TYPE_VIDEO_CONTROLER))

#define GMC_VIDEO_CONTROLER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  GMC_TYPE_VIDEO_CONTROLER, GMCVideoControlerClass))

typedef struct _GMCVideoControler        GMCVideoControler;
typedef struct _GMCVideoControlerPrivate GMCVideoControlerPrivate;
typedef struct _GMCVideoControlerClass   GMCVideoControlerClass;

struct _GMCVideoControler
{
  ClutterGroup              parent;
  
  /*< priv >*/
  GMCVideoControlerPrivate    *priv;
};

struct _GMCVideoControlerClass 
{
  ClutterGroupClass parent_class;

  /* padding for future expansion */
  void (*_clutter_reflect_1) (void);
  void (*_clutter_reflect_2) (void);
  void (*_clutter_reflect_3) (void);
  void (*_clutter_reflect_4) (void);
}; 

GType           gmc_video_controler_get_type           (void) G_GNUC_CONST;

ClutterActor *  gmc_video_controler_new                (ClutterMedia *media);

G_END_DECLS
