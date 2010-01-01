#ifndef _GMC_BUTTON_PLAY
#define _GMC_BUTTON_PLAY

#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GMC_TYPE_BUTTON_PLAY gmc_button_play_get_type()

#define GMC_BUTTON_PLAY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GMC_TYPE_BUTTON_PLAY, GMCButtonPlay))

#define GMC_BUTTON_PLAY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GMC_TYPE_BUTTON_PLAY, GMCButtonPlayClass))

#define GMC_IS_BUTTON_PLAY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMC_TYPE_BUTTON_PLAY))

#define GMC_IS_BUTTON_PLAY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GMC_TYPE_BUTTON_PLAY))

#define GMC_BUTTON_PLAY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GMC_TYPE_BUTTON_PLAY, GMCButtonPlayClass))

typedef struct {
  ClutterCairoTexture parent;
} GMCButtonPlay;

typedef struct {
  ClutterCairoTextureClass parent_class;
} GMCButtonPlayClass;

GType gmc_button_play_get_type (void);

ClutterActor* gmc_button_play_new (void);

G_END_DECLS

#endif /* _GMC_BUTTON_PLAY */