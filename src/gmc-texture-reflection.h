#ifndef __GMC_TEXTURE_REFLECTION_H__
#define __GMC_TEXTURE_REFLECTION_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GMC_TYPE_TEXTURE_REFLECTION            (gmc_texture_reflection_get_type ())
#define GMC_TEXTURE_REFLECTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GMC_TYPE_TEXTURE_REFLECTION, GMCTextureReflection))
#define GMC_IS_TEXTURE_REFLECTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMC_TYPE_TEXTURE_REFLECTION))
#define GMC_TEXTURE_REFLECTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GMC_TYPE_TEXTURE_REFLECTION, GMCTextureReflectionClass))
#define GMC_IS_TEXTURE_REFLECTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GMC_TYPE_TEXTURE_REFLECTION))
#define GMC_TEXTURE_REFLECTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GMC_TYPE_TEXTURE_REFLECTION, GMCTextureReflectionClass))

typedef struct _GMCTextureReflection           GMCTextureReflection;
typedef struct _GMCTextureReflectionPrivate    GMCTextureReflectionPrivate;
typedef struct _GMCTextureReflectionClass      GMCTextureReflectionClass;

struct _GMCTextureReflection
{
  ClutterClone parent_instance;

  GMCTextureReflectionPrivate *priv;
};

struct _GMCTextureReflectionClass
{
  ClutterCloneClass parent_class;
};

GType         gmc_texture_reflection_get_type              (void) G_GNUC_CONST;
ClutterActor *gmc_texture_reflection_new                   (ClutterTexture        *parent_texture);
void          gmc_texture_reflection_set_reflection_height (GMCTextureReflection *texture,
                                                             gint                   height);
gint          gmc_texture_reflection_get_reflection_height (GMCTextureReflection *texture);

G_END_DECLS

#endif /* __GMC_TEXTURE_REFLECTION_H__ */
