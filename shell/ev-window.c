/* this file is part of evince, a gnome document viewer
 *
 *  Copyright (C) 2004 Martin Kretzschmar
 *
 *  Author:
 *    Martin Kretzschmar <martink@gnome.org>
 *
 * Evince is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Evince is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ev-window.h"

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>
#include <gtk/gtkaboutdialog.h>
#include <gtk/gtkaccelgroup.h>
#include <gtk/gtkactiongroup.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkstatusbar.h>
#include <gtk/gtkstock.h>
#include <gtk/gtktoggleaction.h>
#include <gtk/gtkuimanager.h>
#include <gtk/gtkvbox.h>
#include <libgnomevfs/gnome-vfs-mime-utils.h>

#include <string.h>

#include "ev-application.h"

enum {
	PROP_0,
	PROP_ATTRIBUTE
};

enum {
	SIGNAL,
	N_SIGNALS
};

struct _EvWindowPrivate {
	GtkWidget *main_box;
	GtkWidget *bonobo_widget;
	GtkUIManager *ui_manager;
	GtkWidget *statusbar;
	guint help_message_cid;
};

#if 0
/* enable these to add support for signals */
static guint ev_window_signals [N_SIGNALS] = { 0 };
#endif

static GObjectClass *parent_class = NULL;

G_DEFINE_TYPE (EvWindow, ev_window, GTK_TYPE_WINDOW)

#define EV_WINDOW_GET_PRIVATE(object) \
	(G_TYPE_INSTANCE_GET_PRIVATE ((object), EV_TYPE_WINDOW, EvWindowPrivate))

#if 0
const char *
ev_window_get_attribute (EvWindow *self)
{
	g_return_val_if_fail (self != NULL && EV_IS_WINDOW (self), NULL);
	
	return self->priv->attribute;
}

void
ev_window_set_attribute (EvWindow* self, const char *attribute)
{
	g_assert (self != NULL && EV_IS_WINDOW (self));
	g_assert (attribute != NULL);

	if (self->priv->attribute != NULL) {
		g_free (self->priv->attribute);
	}

	self->priv->attribute = g_strdup (attribute);

	g_object_notify (G_OBJECT (self), "attribute");
}

static void
ev_window_get_property (GObject *object, guint prop_id, GValue *value,
			GParamSpec *param_spec)
{
	EvWindow *self;

	self = EV_WINDOW (object);

	switch (prop_id) {
	case PROP_ATTRIBUTE:
		g_value_set_string (value, self->priv->attribute);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object,
						   prop_id,
						   param_spec);
		break;
	}
}

static void
ev_window_set_property (GObject *object, guint prop_id, const GValue *value,
			GParamSpec *param_spec)
{
	EvWindow *self;
	
	self = EV_WINDOW (object);
	
	switch (prop_id) {
	case PROP_ATTRIBUTE:
		ev_window_set_attribute (self, g_value_get_string (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object,
						   prop_id,
						   param_spec);
		break;
	}
}
#endif

gboolean
ev_window_is_empty (const EvWindow *ev_window)
{
	g_return_val_if_fail (EV_IS_WINDOW (ev_window), FALSE);
	
	return ev_window->priv->bonobo_widget == NULL;
}

void
ev_window_open (EvWindow *ev_window, const char *uri)
{
#if 0
	char *mime_type;
	BonoboObject *bonobo_control;
	CORBA_Environment ev;
	Bonobo_PersistFile pf;

	mime_type = gnome_vfs_get_mime_type (uri);

	g_return_if_fail (mime_type != NULL); /* FIXME set error */

	if (!strcmp (mime_type, "application/pdf")) {
		bonobo_control = create_gpdf_control ();
	} else if (!strcmp (mime_type, "application/postscript")) {
		bonobo_control = create_ggv_control ();
	} else if (!strcmp (mime_type, "application/x-gzip")) {
		g_message ("Cannot open gzip-compressed file %s.", uri);
		goto finally;
	} else if (!strcmp (mime_type, "application/x-bzip")) {
		g_message ("Cannot open bzip2-compressed file %s.", uri);
		goto finally;
	} else {
		g_warning ("Don't know how to open %s file %s.",
			   mime_type, uri); /* FIXME set error */
		goto finally;
	}

	ev_window->priv->bonobo_widget = bonobo_widget_new_control_from_objref (
		bonobo_object_corba_objref (bonobo_control), CORBA_OBJECT_NIL);
	gtk_box_pack_start (GTK_BOX (ev_window->priv->main_box),
			    ev_window->priv->bonobo_widget,
			    TRUE, TRUE, 0);
	CORBA_exception_init (&ev);
	pf = bonobo_object_query_interface (
		bonobo_control, "IDL:Bonobo/PersistFile:1.0", &ev);
	Bonobo_PersistFile_load (pf, uri, &ev);
	gtk_widget_show (ev_window->priv->bonobo_widget);
	bonobo_object_release_unref (pf, &ev);
	bonobo_object_unref (bonobo_control);
	CORBA_exception_free (&ev);

finally:
	g_free (mime_type);
#endif
}

static void
ev_window_cmd_file_open (GtkAction *action, EvWindow *ev_window)
{
	ev_application_open (EV_APP, NULL);
}

static void
ev_window_cmd_file_close_window (GtkAction *action, EvWindow *ev_window)
{
	g_return_if_fail (EV_IS_WINDOW (ev_window));

	gtk_widget_destroy (GTK_WIDGET (ev_window));
}

static void
ev_window_cmd_help_about (GtkAction *action, EvWindow *ev_window)
{
	const char *authors[] = {
		N_("Many..."),
		NULL
	};

	const char *documenters[] = {
		N_("Not so many..."),
		NULL
	};

	const char *license[] = {
		N_("Evince is free software; you can redistribute it and/or modify\n"
		   "it under the terms of the GNU General Public License as published by\n"
		   "the Free Software Foundation; either version 2 of the License, or\n"
		   "(at your option) any later version.\n"),		
		N_("Evince is distributed in the hope that it will be useful,\n"
		   "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		   "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		   "GNU General Public License for more details.\n"),
		N_("You should have received a copy of the GNU General Public License\n"
		   "along with Evince; if not, write to the Free Software Foundation, Inc.,\n"
		   "59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n")
	};

	char *license_trans;

#ifdef ENABLE_NLS
	const char **p;

	for (p = authors; *p; ++p)
		*p = _(*p);

	for (p = documenters; *p; ++p)
		*p = _(*p);
#endif

	license_trans = g_strconcat (_(license[0]), "\n", _(license[1]), "\n",
				     _(license[2]), "\n", NULL);

	gtk_show_about_dialog (
		GTK_WINDOW (ev_window),
		"name", _("Evince"),
		"version", VERSION,
		"copyright",
		_("\xc2\xa9 1996-2004 The Evince authors"),
		"license", license_trans,
		"website", "http://www.gnome.org/projects/evince",
		"comments", _("PostScript and PDF File Viewer."),
		"authors", authors,
		"documenters", documenters,
		"translator-credits", _("translator-credits"),
		NULL);

	g_free (license_trans);
}

static void
ev_window_view_toolbar_cb (GtkAction *action, EvWindow *ev_window)
{
	g_object_set (
		G_OBJECT (gtk_ui_manager_get_widget (
				  ev_window->priv->ui_manager,
				  "/ToolBar")),
		"visible",
		gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)),
		NULL);
}

static void
ev_window_view_statusbar_cb (GtkAction *action, EvWindow *ev_window)
{
	g_object_set (
		ev_window->priv->statusbar,
		"visible",
		gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)),
		NULL);
}

static void
menu_item_select_cb (GtkMenuItem *proxy, EvWindow *ev_window)
{
	GtkAction *action;
	char *message;

	action = g_object_get_data (G_OBJECT (proxy), "gtk-action");
	g_return_if_fail (action != NULL);
	
	g_object_get (G_OBJECT (action), "tooltip", &message, NULL);
	if (message) {
		gtk_statusbar_push (GTK_STATUSBAR (ev_window->priv->statusbar),
				    ev_window->priv->help_message_cid, message);
		g_free (message);
	}
}

static void
menu_item_deselect_cb (GtkMenuItem *proxy, EvWindow *ev_window)
{
	gtk_statusbar_pop (GTK_STATUSBAR (ev_window->priv->statusbar),
			   ev_window->priv->help_message_cid);
}

static void
connect_proxy_cb (GtkUIManager *ui_manager, GtkAction *action,
		  GtkWidget *proxy, EvWindow *ev_window)
{
	if (GTK_IS_MENU_ITEM (proxy)) {
		g_signal_connect (proxy, "select",
				  G_CALLBACK (menu_item_select_cb), ev_window);
		g_signal_connect (proxy, "deselect",
				  G_CALLBACK (menu_item_deselect_cb),
				  ev_window);
	}
}

static void
disconnect_proxy_cb (GtkUIManager *ui_manager, GtkAction *action,
		     GtkWidget *proxy, EvWindow *ev_window)
{
	if (GTK_IS_MENU_ITEM (proxy)) {
		g_signal_handlers_disconnect_by_func
			(proxy, G_CALLBACK (menu_item_select_cb), ev_window);
		g_signal_handlers_disconnect_by_func
			(proxy, G_CALLBACK (menu_item_deselect_cb), ev_window);
	}
}

static void
ev_window_dispose (GObject *object)
{
	EvWindowPrivate *priv;

	g_return_if_fail (object != NULL && EV_IS_WINDOW (object));

	priv = EV_WINDOW (object)->priv;

	g_object_unref (priv->ui_manager);

	G_OBJECT_CLASS (parent_class)->dispose (object);
}

static void
ev_window_class_init (EvWindowClass *ev_window_class)
{
	GObjectClass *g_object_class;

	parent_class = g_type_class_peek_parent (ev_window_class);

	g_object_class = G_OBJECT_CLASS (ev_window_class);
	g_object_class->dispose = ev_window_dispose;

	g_type_class_add_private (g_object_class, sizeof (EvWindowPrivate));

#if 0
	/* setting up signal system */
	ev_window_class->signal = ev_window_signal;

	ev_window_signals [SIGNAL] = g_signal_new (
		"signal",
		EV_TYPE_WINDOW,
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET (EvWindowClass,
				 signal),
		NULL,
		NULL,
		g_cclosure_marshal_VOID__STRING,
		G_TYPE_NONE,
		0);
	/* setting up property system */
	g_object_class->set_property = ev_window_set_property;
	g_object_class->get_property = ev_window_get_property;

	g_object_class_install_property (
		g_object_class,
		PROP_ATTRIBUTE,
		g_param_spec_string ("attribute",
				     "Attribute",
				     "A simple unneccessary attribute that "
				     "does nothing special except being a "
				     "demonstration for the correct implem"
				     "entation of a GObject property",
				     "default_value",
				     G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
#endif
}

/* Normal items */
static GtkActionEntry entries[] = {
	{ "File", NULL, N_("_File") },
	{ "View", NULL, N_("_View") },
	{ "Help", NULL, N_("_Help") },

	/* File menu */
	{ "FileOpen", GTK_STOCK_OPEN, N_("_Open"), "<control>O",
	  N_("Open a file"),
	  G_CALLBACK (ev_window_cmd_file_open) },
	{ "FileCloseWindow", GTK_STOCK_CLOSE, N_("_Close"), "<control>W",
	  N_("Close this window"),
	  G_CALLBACK (ev_window_cmd_file_close_window) },

	/* Help menu */
	{ "HelpAbout", NULL, N_("_About"), NULL,
	  N_("Display credits for the document viewer creators"),
	  G_CALLBACK (ev_window_cmd_help_about) },
};

/* Toggle items */
static GtkToggleActionEntry toggle_entries[] = {
	/* View Menu */
	{ "ViewToolbar", NULL, N_("_Toolbar"), "<shift><control>T",
	  N_("Show or hide toolbar"),
	  G_CALLBACK (ev_window_view_toolbar_cb), TRUE },
	{ "ViewStatusbar", NULL, N_("_Statusbar"), NULL,
	  N_("Show or hide statusbar"),
	  G_CALLBACK (ev_window_view_statusbar_cb), TRUE },
};

static void
ev_window_init (EvWindow *ev_window)
{
	GtkActionGroup *action_group;
	GtkAccelGroup *accel_group;
	GError *error = NULL;
	GtkWidget *menubar;
	GtkWidget *toolbar;

	ev_window->priv = EV_WINDOW_GET_PRIVATE (ev_window);

	gtk_window_set_title (GTK_WINDOW (ev_window), _("Evince"));

	ev_window->priv->main_box = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (ev_window), ev_window->priv->main_box);
	gtk_widget_show (ev_window->priv->main_box);

	action_group = gtk_action_group_new ("MenuActions");
	gtk_action_group_set_translation_domain (action_group, NULL);
	gtk_action_group_add_actions (action_group, entries,
				      G_N_ELEMENTS (entries), ev_window);
	gtk_action_group_add_toggle_actions (action_group, toggle_entries,
					     G_N_ELEMENTS (toggle_entries),
					     ev_window);

	ev_window->priv->ui_manager = gtk_ui_manager_new ();
	gtk_ui_manager_insert_action_group (ev_window->priv->ui_manager,
					    action_group, 0);

	accel_group =
		gtk_ui_manager_get_accel_group (ev_window->priv->ui_manager);
	gtk_window_add_accel_group (GTK_WINDOW (ev_window), accel_group);

	g_signal_connect (ev_window->priv->ui_manager, "connect_proxy",
			  G_CALLBACK (connect_proxy_cb), ev_window);
	g_signal_connect (ev_window->priv->ui_manager, "disconnect_proxy",
			  G_CALLBACK (disconnect_proxy_cb), ev_window);

	if (!gtk_ui_manager_add_ui_from_file (ev_window->priv->ui_manager,
					      DATADIR"/evince-ui.xml",
					      &error)) {
		g_message ("building menus failed: %s", error->message);
		g_error_free (error);
	}

	menubar = gtk_ui_manager_get_widget (ev_window->priv->ui_manager,
					     "/MainMenu");
	gtk_box_pack_start (GTK_BOX (ev_window->priv->main_box), menubar,
			    FALSE, FALSE, 0);

	toolbar = gtk_ui_manager_get_widget (ev_window->priv->ui_manager,
					     "/ToolBar");
	gtk_box_pack_start (GTK_BOX (ev_window->priv->main_box), toolbar,
			    FALSE, FALSE, 0);

	ev_window->priv->statusbar = gtk_statusbar_new ();
	gtk_widget_show (ev_window->priv->statusbar);
	gtk_box_pack_end (GTK_BOX (ev_window->priv->main_box),
			  ev_window->priv->statusbar,
			  FALSE, TRUE, 0);
	ev_window->priv->help_message_cid = gtk_statusbar_get_context_id
		(GTK_STATUSBAR (ev_window->priv->statusbar), "help_message");

}
