/*
 * Copyright (C) 2022 The GNOME project contributors
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "nautilus-name-cell.h"
#include "nautilus-file-utilities.h"

struct _NautilusNameCell
{
    NautilusViewCell parent_instance;

    GSignalGroup *item_signal_group;

    GQuark path_attribute_q;
    GFile *file_path_base_location;

    GtkWidget *fixed_height_box;
    GtkWidget *icon;
    GtkWidget *label;
    GtkWidget *snippet;
    GtkWidget *path;

    gboolean show_snippet;
};

G_DEFINE_TYPE (NautilusNameCell, nautilus_name_cell, NAUTILUS_TYPE_VIEW_CELL)

static gchar *
get_path_text (NautilusFile *file,
               GQuark        path_attribute_q,
               GFile        *base_location)
{
    gchar *path_text;
    g_autofree gchar *path = NULL;
    g_autoptr (GFile) dir_location = NULL;
    g_autoptr (GFile) home_location = g_file_new_for_path (g_get_home_dir ());

    if (path_attribute_q == 0)
    {
        return NULL;
    }

    path = nautilus_file_get_string_attribute_q (file, path_attribute_q);
    dir_location = g_file_new_for_commandline_arg (path);

    if (g_file_equal (base_location, dir_location))
    {
        /* Only occurs when search result is
         * a direct child of the base location
         */
        return NULL;
    }

    if (g_file_has_prefix (dir_location, base_location))
    {
        g_autofree gchar *relative_path = NULL;

        relative_path = g_file_get_relative_path (base_location, dir_location);
        path_text = g_filename_display_name (relative_path);
    }
    else if (g_file_equal (dir_location, home_location))
    {
        path_text = nautilus_compute_title_for_location (home_location);
    }
    else if (base_location == NULL &&
             g_file_has_prefix (dir_location, home_location))
    {
        g_autofree gchar *relative_path = NULL;

        relative_path = g_file_get_relative_path (home_location, dir_location);
        path_text = g_filename_display_name (relative_path);
    }
    else
    {
        path_text = g_file_get_path (dir_location);
    }

    return path_text;
}

static gchar *
get_fts_snippet (NautilusFile *file)
{
    const gchar *snippet;
    g_autoptr (GRegex) regex = NULL;

    snippet = nautilus_file_get_search_fts_snippet (file);
    if (snippet == NULL)
    {
        return NULL;
    }

    /* Flatten the text by replacing new lines with spaces */
    regex = g_regex_new ("\\R+", 0, G_REGEX_MATCH_NEWLINE_ANY, NULL);
    return g_regex_replace (regex,
                            snippet,
                            -1,
                            0,
                            " ",
                            G_REGEX_MATCH_NEWLINE_ANY,
                            NULL);
}

static void
update_labels (NautilusNameCell *self)
{
    NautilusViewItem *item;
    NautilusFile *file;
    g_autofree gchar *display_name = NULL;
    g_autofree gchar *path_text = NULL;
    g_autofree gchar *fts_snippet = NULL;

    item = nautilus_view_cell_get_item (NAUTILUS_VIEW_CELL (self));
    g_return_if_fail (item != NULL);
    file = nautilus_view_item_get_file (item);

    display_name = nautilus_file_get_display_name (file);
    path_text = get_path_text (file,
                               self->path_attribute_q,
                               self->file_path_base_location);
    if (self->show_snippet)
    {
        fts_snippet = get_fts_snippet (file);
    }

    gtk_label_set_text (GTK_LABEL (self->label), display_name);
    gtk_label_set_text (GTK_LABEL (self->path), path_text);
    gtk_label_set_text (GTK_LABEL (self->snippet), fts_snippet);

    gtk_widget_set_visible (self->path, (path_text != NULL));
    gtk_widget_set_visible (self->snippet, (fts_snippet != NULL));
}

static void
update_icon (NautilusNameCell *self)
{
    NautilusFileIconFlags flags;
    g_autoptr (GdkPaintable) icon_paintable = NULL;
    GtkStyleContext *style_context;
    NautilusViewItem *item;
    NautilusFile *file;
    guint icon_size;
    int icon_height;
    int extra_margin;
    g_autofree gchar *thumbnail_path = NULL;

    item = nautilus_view_cell_get_item (NAUTILUS_VIEW_CELL (self));
    g_return_if_fail (item != NULL);

    file = nautilus_view_item_get_file (item);
    icon_size = nautilus_view_item_get_icon_size (item);
    flags = NAUTILUS_FILE_ICON_FLAGS_USE_THUMBNAILS |
            NAUTILUS_FILE_ICON_FLAGS_FORCE_THUMBNAIL_SIZE |
            NAUTILUS_FILE_ICON_FLAGS_USE_EMBLEMS |
            NAUTILUS_FILE_ICON_FLAGS_USE_ONE_EMBLEM;

    icon_paintable = nautilus_file_get_icon_paintable (file, icon_size, 1, flags);
    gtk_picture_set_paintable (GTK_PICTURE (self->icon), icon_paintable);

    /* Set the same width for all icons regardless of aspect ratio.
     * Don't set the width here because it would get GtkPicture w4h confused.
     */
    gtk_widget_set_size_request (self->fixed_height_box, icon_size, -1);

    /* Give all items the same minimum width. This cannot be done by setting the
     * width request directly, as above, because it would get mess up with
     * height for width calculations.
     *
     * Instead we must add margins on both sides of the icon which, summed up
     * with the icon's actual width, equal the desired item width. */
    icon_height = gdk_paintable_get_intrinsic_height (icon_paintable);
    extra_margin = (icon_size - icon_height) / 2;
    gtk_widget_set_margin_top (self->fixed_height_box, extra_margin);
    gtk_widget_set_margin_bottom (self->fixed_height_box, extra_margin);

    style_context = gtk_widget_get_style_context (self->icon);
    thumbnail_path = nautilus_file_get_thumbnail_path (file);
    if (icon_size >= NAUTILUS_THUMBNAIL_MINIMUM_ICON_SIZE &&
        thumbnail_path != NULL &&
        nautilus_file_should_show_thumbnail (file))
    {
        gtk_style_context_add_class (style_context, "thumbnail");
    }
    else
    {
        gtk_style_context_remove_class (style_context, "thumbnail");
    }
}

static void
on_file_changed (NautilusNameCell *self)
{
    update_icon (self);
    update_labels (self);
}

static void
on_item_size_changed (NautilusNameCell *self)
{
    update_icon (self);
}

static void
on_item_is_cut_changed (NautilusNameCell *self)
{
    gboolean is_cut;

    g_object_get (self, "is-cut", &is_cut, NULL);
    if (is_cut)
    {
        gtk_widget_add_css_class (self->icon, "cut");
    }
    else
    {
        gtk_widget_remove_css_class (self->icon, "cut");
    }
}

static void
nautilus_name_cell_init (NautilusNameCell *self)
{
    gtk_widget_init_template (GTK_WIDGET (self));

    /* Connect automatically to an item. */
    self->item_signal_group = g_signal_group_new (NAUTILUS_TYPE_VIEW_ITEM);
    g_signal_group_connect_swapped (self->item_signal_group, "notify::icon-size",
                                    (GCallback) on_item_size_changed, self);
    g_signal_group_connect_swapped (self->item_signal_group, "notify::is-cut",
                                    (GCallback) on_item_is_cut_changed, self);
    g_signal_group_connect_swapped (self->item_signal_group, "file-changed",
                                    (GCallback) on_file_changed, self);
    g_signal_connect_object (self->item_signal_group, "bind",
                             (GCallback) on_file_changed, self,
                             G_CONNECT_SWAPPED);

    g_object_bind_property (self, "item",
                            self->item_signal_group, "target",
                            G_BINDING_SYNC_CREATE);
}

static void
nautilus_name_cell_finalize (GObject *object)
{
    NautilusNameCell *self = (NautilusNameCell *) object;

    g_clear_object (&self->item_signal_group);
    g_clear_object (&self->file_path_base_location);
    G_OBJECT_CLASS (nautilus_name_cell_parent_class)->finalize (object);
}

static void
nautilus_name_cell_class_init (NautilusNameCellClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    object_class->finalize = nautilus_name_cell_finalize;

    gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/nautilus/ui/nautilus-name-cell.ui");

    gtk_widget_class_bind_template_child (widget_class, NautilusNameCell, fixed_height_box);
    gtk_widget_class_bind_template_child (widget_class, NautilusNameCell, icon);
    gtk_widget_class_bind_template_child (widget_class, NautilusNameCell, label);
    gtk_widget_class_bind_template_child (widget_class, NautilusNameCell, snippet);
    gtk_widget_class_bind_template_child (widget_class, NautilusNameCell, path);
}

NautilusViewCell *
nautilus_name_cell_new (NautilusListBase *view)
{
    return NAUTILUS_VIEW_CELL (g_object_new (NAUTILUS_TYPE_NAME_CELL,
                                             "view", view,
                                             NULL));
}

void
nautilus_name_cell_set_path (NautilusNameCell *self,
                             GQuark            path_attribute_q,
                             GFile            *base_location)
{
    self->path_attribute_q = path_attribute_q;
    g_set_object (&self->file_path_base_location, base_location);
}

void
nautilus_name_cell_show_snippet (NautilusNameCell *self)
{
    self->show_snippet = TRUE;
}
