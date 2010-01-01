#include <cogl/cogl.h>
#include <clutter/clutter.h>

#include "gmc-texture-reflection.h"

enum
{
  PROP_0,

  PROP_REFLECTION_HEIGHT
};

#define GMC_TEXTURE_REFLECTION_GET_PRIVATE(obj)        \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj),            \
         GMC_TYPE_TEXTURE_REFLECTION,                  \
         GMCTextureReflectionPrivate))

struct _GMCTextureReflectionPrivate
{
  gint reflection_height;
};

G_DEFINE_TYPE (GMCTextureReflection,
               gmc_texture_reflection,
               CLUTTER_TYPE_CLONE);

static void
gmc_texture_reflection_paint (ClutterActor *actor)
{
  GMCTextureReflectionPrivate *priv;
  GMCTextureReflection *texture;
  ClutterClone		    *clone;
  ClutterActor        *parent;
  gfloat                  width, height;
  gint                   r_height;  

  CoglHandle        cogl_texture;
  CoglTextureVertex tvert[4];
  CoglFixed      rty;

  texture = GMC_TEXTURE_REFLECTION (actor);
  clone = CLUTTER_CLONE (actor);

  parent = clutter_clone_get_source (clone);
  if (!parent)
    return;

  if (!CLUTTER_ACTOR_IS_REALIZED (parent))
    clutter_actor_realize (CLUTTER_ACTOR (parent));

  cogl_texture = clutter_texture_get_cogl_texture (CLUTTER_TEXTURE (parent));
  if (cogl_texture == COGL_INVALID_HANDLE)
    return;

  priv = texture->priv;

  clutter_actor_get_size (actor, &width, &height);

  r_height = priv->reflection_height;

  if (r_height < 0 || r_height > height)
    r_height = height;

#define FX(x) COGL_FIXED_FROM_FLOAT(x)

  rty = COGL_FIXED_DIV(FX(r_height),FX(height));

  /* clockise vertices and tex coords and colors! */

  tvert[0].x = tvert[0].y = tvert[0].z = 0; 
  tvert[0].tx = 0; tvert[0].ty = rty;  
  tvert[0].color.red = tvert[0].color.green = tvert[0].color.blue = 0xff;
  tvert[0].color.alpha = clutter_actor_get_opacity (actor);

  tvert[1].x = FX(width); tvert[1].y = tvert[1].z = 0; 
  tvert[1].tx = COGL_FIXED_1; tvert[1].ty = rty;  
  tvert[1].color.red = tvert[1].color.green = tvert[1].color.blue = 0xff;
  tvert[1].color.alpha = clutter_actor_get_opacity (actor);

  tvert[2].x = FX(width); tvert[2].y = FX(r_height); tvert[2].z = 0; 
  tvert[2].tx = COGL_FIXED_1; tvert[2].ty = 0;  
  tvert[2].color.red = tvert[2].color.green = tvert[2].color.blue = 0xff;
  tvert[2].color.alpha = 0;

  tvert[3].x = 0; tvert[3].y = FX(r_height); tvert[3].z = 0; 
  tvert[3].tx = 0; tvert[3].ty = 0;  
  tvert[3].color.red = tvert[3].color.green = tvert[3].color.blue = 0xff;
  tvert[3].color.alpha = 0;

	cogl_push_matrix ();

	cogl_set_source_texture (cogl_texture);
	cogl_polygon (tvert, 4, FALSE);

	cogl_pop_matrix ();
}

static void
gmc_texture_reflection_set_property (GObject      *gobject,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  GMCTextureReflection *texture;

  texture = GMC_TEXTURE_REFLECTION (gobject);

  switch (prop_id)
    {
    case PROP_REFLECTION_HEIGHT:
      gmc_texture_reflection_set_reflection_height (texture,
                                                     g_value_get_int (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
gmc_texture_reflection_get_property (GObject    *gobject,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  GMCTextureReflection *texture;

  texture = GMC_TEXTURE_REFLECTION (gobject);

  switch (prop_id)
    {
    case PROP_REFLECTION_HEIGHT:
      g_value_set_int (value,
                       gmc_texture_reflection_get_reflection_height (texture));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
gmc_texture_reflection_class_init (GMCTextureReflectionClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GMCTextureReflectionPrivate));

  gobject_class->set_property = gmc_texture_reflection_set_property;
  gobject_class->get_property = gmc_texture_reflection_get_property;

  actor_class->paint = gmc_texture_reflection_paint;

  g_object_class_install_property (gobject_class,
                                   PROP_REFLECTION_HEIGHT,
                                   g_param_spec_int ("reflection-height",
                                                     "Reflection Height",
                                                     "Height of the reflection, or -1",
                                                     -1, G_MAXINT, -1,
                                                     G_PARAM_READABLE | G_PARAM_WRITABLE));
}

static void
gmc_texture_reflection_init (GMCTextureReflection *texture)
{
  GMCTextureReflectionPrivate *priv;

  texture->priv = priv = GMC_TEXTURE_REFLECTION_GET_PRIVATE (texture);
  
  priv->reflection_height = -1;
}

ClutterActor *
gmc_texture_reflection_new (ClutterTexture *texture)
{
  g_return_val_if_fail (texture == NULL || CLUTTER_IS_TEXTURE (texture), NULL);

  return g_object_new (GMC_TYPE_TEXTURE_REFLECTION,
                       "source", texture,
                       NULL);
}

void
gmc_texture_reflection_set_reflection_height (GMCTextureReflection *texture,
                                               gint                   height)
{
  GMCTextureReflectionPrivate *priv;

  g_return_if_fail (GMC_IS_TEXTURE_REFLECTION (texture));

  priv = texture->priv;

  if (priv->reflection_height != height)
    {
      priv->reflection_height = height;

      if (CLUTTER_ACTOR_IS_VISIBLE (texture))
        clutter_actor_queue_redraw (CLUTTER_ACTOR (texture));

      g_object_notify (G_OBJECT (texture), "reflection-height");
    }
}

gint
gmc_texture_reflection_get_reflection_height (GMCTextureReflection *texture)
{
  g_return_val_if_fail (GMC_IS_TEXTURE_REFLECTION (texture), -1);

  return texture->priv->reflection_height;
}
