#include "gmc-video-model.h"
#include <sqlite3.h>
#include <string.h>

G_DEFINE_TYPE (GmcVideoModel, gmc_video_model, CLUTTER_TYPE_MODEL);

#define GMC_VIDEO_MODEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GMC_TYPE_VIDEO_MODEL, GmcVideoModelPrivate))

static const gchar *table_name = "Movie";

struct _GmcVideoModelPrivate
{
  gchar *filename;
  sqlite3 *sql;
};

enum {
  PROP_0,
  PROP_FILENAME
};

typedef enum
{
  COLUMN_ACTORS,
  COLUMN_DURATION,
  COLUMN_GENRE,
  COLUMN_RESUME,
  COLUMN_TITLE,
  COLUMN_YEAR
} Columns;

const struct {
  const gchar *name;
  GType type;
} ColumnNames [] = {
  { "Actors", G_TYPE_POINTER },
  { "Duration", G_TYPE_UINT },
  { "Genre", G_TYPE_STRING },
  { "Resume", G_TYPE_STRING },
  { "Title", G_TYPE_STRING },
  { "Year", G_TYPE_UINT }
};

void
gmc_video_model_set_filename (GmcVideoModel *self, const gchar *filename)
{
  GmcVideoModelPrivate *priv;
  //sqlite3_stmt *statement;
  gint error_code;

  priv = GMC_VIDEO_MODEL_GET_PRIVATE (self);

  if (g_strcmp0 (priv->filename, filename) == 0) {
    return;
  }

  if (priv->filename) {
    g_free (priv->filename);
  }
  priv->filename = g_strdup (filename);

  if (priv->sql) {
    error_code = sqlite3_close (priv->sql);
    if (error_code != SQLITE_OK) {
      g_critical ("%s [close] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    }
  }

  error_code = sqlite3_open_v2 (priv->filename, &priv->sql, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if (error_code != SQLITE_OK) {
    g_critical ("%s [open] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return;
  }

/*  gchar *query = "CREATE TABLE Movies ( Title CHAR(50) PRIMARY KEY, Year INTEGER(4) );";
  error_code = sqlite3_prepare_v2 (priv->sql, query, strlen (query), &statement, NULL);
  if (error_code != SQLITE_OK) {
    g_critical ("%s prepare : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return;
  }

  error_code = sqlite3_step (statement);
  if (error_code != SQLITE_DONE) {
    g_critical ("%s step : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return;
  }

  sqlite3_finalize (statement);*/
}

static void
gmc_video_model_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  GmcVideoModel *self;

  self = GMC_VIDEO_MODEL (object);

  switch (prop_id)
  {
    case PROP_FILENAME:
      gmc_video_model_set_filename (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gmc_video_model_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  GmcVideoModel *self;
  GmcVideoModelPrivate *priv;

  self = GMC_VIDEO_MODEL (object);
  priv = GMC_VIDEO_MODEL_GET_PRIVATE (priv);

  switch (prop_id)
  {
    case PROP_FILENAME:
      g_value_set_string (value, priv->filename);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static guint
gmc_video_model_get_n_rows (ClutterModel *model)
{
  GmcVideoModel *self;
  GmcVideoModelPrivate *priv;
  sqlite3_stmt *statement;
  gint error_code;
  guint n_rows;
  gchar *query;

  self = GMC_VIDEO_MODEL (model);
  priv = GMC_VIDEO_MODEL_GET_PRIVATE (self);

  query = "SELECT COUNT (*) FROM Movie";
  error_code = sqlite3_prepare_v2 (priv->sql, query, strlen (query), &statement, NULL);
  if (error_code != SQLITE_OK) {
    g_critical ("%s [prepare] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return 0;
  }

  error_code = sqlite3_step (statement);
  if (error_code != SQLITE_ROW) {
    g_critical ("%s [step] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return 0;
  }

  n_rows = sqlite3_column_int (statement, 0);

  sqlite3_finalize (statement);

  return n_rows;
}

static guint
gmc_video_model_get_n_columns (ClutterModel *model)
{
  return G_N_ELEMENTS (ColumnNames);
}

static const gchar *
gmc_video_model_get_column_name (ClutterModel *model,
                                 guint         column)
{
  return ColumnNames[column].name;
}

static GType
gmc_video_model_get_column_type (ClutterModel *model,
                                  guint         column)
{
  return ColumnNames[column].type;
}

static ClutterModelIter *
gmc_video_model_insert_row (ClutterModel *model,
                            gint          index)
{
  GmcVideoModel *self;
  GmcVideoModelPrivate *priv;
  sqlite3_stmt *statement;
  gint error_code;
  gchar *query;

  self = GMC_VIDEO_MODEL (model);
  priv = GMC_VIDEO_MODEL_GET_PRIVATE (self);

  g_sprintf (query, "INSERT INTO %s (Title, Year) VALUES (%s, %d)", table_name, "_", "0");
  g_debug ("%s [query] : %s", G_STRFUNC, query);

  error_code = sqlite3_prepare_v2 (priv->sql, query, strlen (query), &statement, NULL);
  if (error_code != SQLITE_OK) {
    g_critical ("%s [prepare] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return 0;
  }

  error_code = sqlite3_step (statement);
  if (error_code != SQLITE_DONE) {
    g_critical ("%s [step] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return 0;
  }

  sqlite3_finalize (statement);

  // create iter
}

static void
gmc_video_model_remove_row (ClutterModel *model,
                            guint         row)
{
  
}

static ClutterModelIter *
gmc_video_model_get_iter_at_row (ClutterModel *model,
                                 guint         row)
{
  
}

static void
gmc_video_model_resort (ClutterModel         *model,
                        ClutterModelSortFunc  func,
                        gpointer              data)
{
  
}

static void
gmc_video_model_class_init (GmcVideoModelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  ClutterModelClass *model_class = CLUTTER_MODEL_CLASS (klass);

  g_type_class_add_private (object_class, sizeof (GmcVideoModelPrivate));

  object_class->set_property = gmc_video_model_set_property;
  object_class->get_property = gmc_video_model_get_property;

  model_class->get_n_rows = gmc_video_model_get_n_rows;
  model_class->get_n_columns = gmc_video_model_get_n_columns;
  model_class->get_column_name = gmc_video_model_get_column_name;
  model_class->get_column_type = gmc_video_model_get_column_type;
  //model_class->insert_row = gmc_video_model_insert_row;
  //model_class->remove_row = gmc_video_model_remove_row;
  //model_class->get_iter_at_row = gmc_video_model_get_iter_at_row;
  //model_class->resort = gmc_video_model_resort;

  g_object_class_install_property (object_class,
                                   PROP_FILENAME,
                                   g_param_spec_string ("filename",
                                                        "Filename",
                                                        "The database filename",
                                                        "movies_default.db",
                                                        G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT));

}

static void
gmc_video_model_init (GmcVideoModel *self)
{
  GmcVideoModelPrivate *priv;
}

ClutterModel *
gmc_video_model_new (const gchar *filename)
{
  GObject *self;

  self = g_object_new (GMC_TYPE_VIDEO_MODEL,
                       "filename", filename,
                       NULL);

  return CLUTTER_MODEL (self);
}
