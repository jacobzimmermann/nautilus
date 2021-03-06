
/*
 * Nautilus
 *
 * Copyright (C) 2002 Sun Microsystems, Inc.
 *
 * Nautilus is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Nautilus is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>.
 * 
 * Author: Dave Camp <dave@ximian.com>
 */

/* nautilus-tree-view-drag-dest.h: Handles drag and drop for treeviews which 
 *                                 contain a hierarchy of files
 */

#pragma once

#if 0 && NAUTILUS_DND_NEEDS_GTK4_REIMPLEMENTATION
#include <gtk/gtk.h>

#include "nautilus-file.h"

G_BEGIN_DECLS

#define NAUTILUS_TYPE_TREE_VIEW_DRAG_DEST	(nautilus_tree_view_drag_dest_get_type ())
#define NAUTILUS_TREE_VIEW_DRAG_DEST(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), NAUTILUS_TYPE_TREE_VIEW_DRAG_DEST, NautilusTreeViewDragDest))
#define NAUTILUS_TREE_VIEW_DRAG_DEST_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), NAUTILUS_TYPE_TREE_VIEW_DRAG_DEST, NautilusTreeViewDragDestClass))
#define NAUTILUS_IS_TREE_VIEW_DRAG_DEST(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), NAUTILUS_TYPE_TREE_VIEW_DRAG_DEST))
#define NAUTILUS_IS_TREE_VIEW_DRAG_DEST_CLASS(klass)	(G_TYPE_CLASS_CHECK_CLASS_TYPE ((klass), NAUTILUS_TYPE_TREE_VIEW_DRAG_DEST))

typedef struct _NautilusTreeViewDragDest        NautilusTreeViewDragDest;
typedef struct _NautilusTreeViewDragDestClass   NautilusTreeViewDragDestClass;
typedef struct _NautilusTreeViewDragDestDetails NautilusTreeViewDragDestDetails;

struct _NautilusTreeViewDragDest {
	GObject parent;
	
	NautilusTreeViewDragDestDetails *details;
};

struct _NautilusTreeViewDragDestClass {
	GObjectClass parent;
	
	char *(*get_root_uri) (NautilusTreeViewDragDest *dest);
	NautilusFile *(*get_file_for_path) (NautilusTreeViewDragDest *dest,
					    GtkTreePath *path);
	void (*move_copy_items) (NautilusTreeViewDragDest *dest,
				 const GList *item_uris,
				 const char *target_uri,
				 GdkDragAction action,
				 int x,
				 int y);
	void (* handle_netscape_url) (NautilusTreeViewDragDest *dest,
				 const char *url,
				 const char *target_uri,
				 GdkDragAction action,
				 int x,
				 int y);
	void (* handle_uri_list) (NautilusTreeViewDragDest *dest,
				  const char *uri_list,
				  const char *target_uri,
				  GdkDragAction action,
				  int x,
				  int y);
	void (* handle_text)    (NautilusTreeViewDragDest *dest,
				  const char *text,
				  const char *target_uri,
				  GdkDragAction action,
				  int x,
				  int y);
	void (* handle_raw)    (NautilusTreeViewDragDest *dest,
				  char *raw_data,
				  int length,
				  const char *target_uri,
				  const char *direct_save_uri,
				  GdkDragAction action,
				  int x,
				  int y);
	void (* handle_hover)   (NautilusTreeViewDragDest *dest,
				 const char *target_uri);
};

GType                     nautilus_tree_view_drag_dest_get_type (void);
NautilusTreeViewDragDest *nautilus_tree_view_drag_dest_new      (GtkTreeView *tree_view);

G_END_DECLS
#endif
