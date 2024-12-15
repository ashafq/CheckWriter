/* checkwriter-application.c
 *
 * Copyright (c) 2024 Ayan Shafqat <ayan@shafq.at>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "config.h"

#include "checkwriter-application.h"
#include "checkwriter-preferences.h"
#include "checkwriter-window.h"

struct _CheckwriterApplication
{
  AdwApplication parent_instance;
};

G_DEFINE_FINAL_TYPE (CheckwriterApplication, checkwriter_application, ADW_TYPE_APPLICATION)

CheckwriterApplication *
checkwriter_application_new (const char *application_id,
                             GApplicationFlags flags)
{
  g_return_val_if_fail (application_id != NULL, NULL);

  return g_object_new (CHECKWRITER_TYPE_APPLICATION,
                       "application-id", application_id,
                       "flags", flags,
                       NULL);
}

static void
checkwriter_application_activate (GApplication *app)
{
  GtkWindow *window;

  g_assert (CHECKWRITER_IS_APPLICATION (app));

  window = gtk_application_get_active_window (GTK_APPLICATION (app));

  if (window == NULL)
    window = g_object_new (CHECKWRITER_TYPE_WINDOW,
                           "application", app,
                           NULL);
  gtk_window_present (window);
}

static void
checkwriter_application_class_init (CheckwriterApplicationClass *klass)
{
  GApplicationClass *app_class = G_APPLICATION_CLASS (klass);

  app_class->activate = checkwriter_application_activate;
}

static void
checkwriter_application_about_action (GSimpleAction *action,
                                      GVariant *parameter,
                                      gpointer user_data)
{
  static const char *developers[] = { "Ayan Shafqat", NULL };
  CheckwriterApplication *self = user_data;
  GtkWidget *window = NULL;

  g_assert (CHECKWRITER_IS_APPLICATION (self));

  /* Explicitly cast GtkWindow to GtkWidget */
  window = GTK_WIDGET (gtk_application_get_active_window (GTK_APPLICATION (self)));

  if (!window)
    {
      g_warning ("No active window found for About dialog");
      return;
    }

  adw_show_about_dialog (window,
                         "application-name", PACKAGE_NAME,
                         "application-icon", PACKAGE_URI,
                         "version", PACKAGE_VERSION,
                         "developers", developers,
                         "copyright", "Copyright © 2024 Ayan Shafqat",
                         NULL);
}

static void
checkwriter_application_quit_action (GSimpleAction *action,
                                     GVariant *parameter,
                                     gpointer user_data)
{
  CheckwriterApplication *self = user_data;

  g_assert (CHECKWRITER_IS_APPLICATION (self));

  g_application_quit (G_APPLICATION (self));
}

static void
checkwriter_application_preferences_action (GSimpleAction *action,
                                            GVariant *parameter,
                                            gpointer user_data)
{
  CheckwriterApplication *self = user_data;
  GtkWindow *parent_window;
  GtkWindow *preferences_window;

  g_assert (CHECKWRITER_IS_APPLICATION (self));

  preferences_window = checkwriter_preferences_window_new (GTK_APPLICATION (self));
  parent_window = gtk_application_get_active_window (GTK_APPLICATION (self));

  if (!preferences_window)
    {
      g_debug ("Error: preferences_window could not be created");
      return;
    }

  if (parent_window)
    {
      gtk_window_set_transient_for (preferences_window, parent_window);
      g_debug ("Set preferences window as transient for the parent window");
    }
  else
    {
      g_warning ("No active parent window, preferences dialog will be standalone");
    }

  gtk_window_set_modal (preferences_window, TRUE);
  gtk_window_present (preferences_window);
}

static const GActionEntry app_actions[] = {
  { "quit", checkwriter_application_quit_action },
  { "about", checkwriter_application_about_action },
  { "preferences", checkwriter_application_preferences_action },
};

static void
checkwriter_application_init (CheckwriterApplication *self)
{
  g_action_map_add_action_entries (G_ACTION_MAP (self),
                                   app_actions,
                                   G_N_ELEMENTS (app_actions),
                                   self);

  gtk_application_set_accels_for_action (GTK_APPLICATION (self),
                                         "app.quit",
                                         (const char *[]){ "<primary>q", NULL });

  gtk_application_set_accels_for_action (GTK_APPLICATION (self),
                                         "app.preferences",
                                         (const char *[]){ "<primary>comma", NULL });
}
