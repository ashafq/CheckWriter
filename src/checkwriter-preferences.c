/*
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

#include "checkwriter-preferences.h"
#include "check-properties.h"

/**
 * Type definitions
 */
struct _CheckwriterPreferences
{
  GtkWidget parent_instance;

  CheckProperties check_properties;

  GtkWindow *preferences_window;

  GtkButton *apply_button;
  GtkButton *cancel_button;

  GtkSpinButton *check_width_spin;
  GtkSpinButton *check_height_spin;
  GtkSpinButton *x_pad_spin;
  GtkSpinButton *y_pad_spin;

  GtkSpinButton *date_x_spin;
  GtkSpinButton *date_y_spin;
  GtkSpinButton *date_width_spin;

  GtkSpinButton *pay_to_x_spin;
  GtkSpinButton *pay_to_y_spin;
  GtkSpinButton *pay_to_width_spin;

  GtkSpinButton *amount_x_spin;
  GtkSpinButton *amount_y_spin;
  GtkSpinButton *amount_width_spin;

  GtkSpinButton *amount_words_x_spin;
  GtkSpinButton *amount_words_y_spin;
  GtkSpinButton *amount_words_width_spin;

  GtkSpinButton *memo_x_spin;
  GtkSpinButton *memo_y_spin;
  GtkSpinButton *memo_width_spin;

  GtkWidget *check_preview_area;
};

G_DEFINE_TYPE (CheckwriterPreferences, checkwriter_preferences, GTK_TYPE_WIDGET)

/***
 * Methods
 */

static void
checkwriter_preferences_load_settings (CheckwriterPreferences *self)
{
  if (!self)
    {
      g_warning ("Self not initialized, not loading settings");
      return;
    }
  check_properties_load (&self->check_properties);

  /* Load current settings */
  gtk_spin_button_set_value (self->check_width_spin, self->check_properties.width);
  gtk_spin_button_set_value (self->check_height_spin, self->check_properties.height);
  gtk_spin_button_set_value (self->x_pad_spin, self->check_properties.x_pad);
  gtk_spin_button_set_value (self->y_pad_spin, self->check_properties.y_pad);

  gtk_spin_button_set_value (self->date_x_spin, self->check_properties.date.x_pos);
  gtk_spin_button_set_value (self->date_y_spin, self->check_properties.date.y_pos);
  gtk_spin_button_set_value (self->date_width_spin, self->check_properties.date.width);

  gtk_spin_button_set_value (self->pay_to_x_spin, self->check_properties.name.x_pos);
  gtk_spin_button_set_value (self->pay_to_y_spin, self->check_properties.name.y_pos);
  gtk_spin_button_set_value (self->pay_to_width_spin, self->check_properties.name.width);

  gtk_spin_button_set_value (self->amount_x_spin, self->check_properties.amount.x_pos);
  gtk_spin_button_set_value (self->amount_y_spin, self->check_properties.amount.y_pos);
  gtk_spin_button_set_value (self->amount_width_spin, self->check_properties.amount.width);

  gtk_spin_button_set_value (self->amount_words_x_spin, self->check_properties.amount_in_words.x_pos);
  gtk_spin_button_set_value (self->amount_words_y_spin, self->check_properties.amount_in_words.y_pos);
  gtk_spin_button_set_value (self->amount_words_width_spin, self->check_properties.amount_in_words.width);

  gtk_spin_button_set_value (self->memo_x_spin, self->check_properties.memo.x_pos);
  gtk_spin_button_set_value (self->memo_y_spin, self->check_properties.memo.y_pos);
  gtk_spin_button_set_value (self->memo_width_spin, self->check_properties.memo.width);
}

/**
 * Event handlers
 */

static void
checkwriter_preferences_on_cancel_button_clicked (GtkButton *button,
                                                  gpointer user_data)
{
  CheckwriterPreferences *self = NULL;

  self = CHECKWRITER_PREFERENCES (user_data);

  if (!self)
    {
      g_error ("%s: Failed to retrieve instance. Invalid user_data parameter.", __func__);
      return;
    }

  g_debug ("%s: Preferences window closed without saving.", __func__);
  gtk_window_destroy (GTK_WINDOW (self->preferences_window));
}

static void
checkwriter_preferences_on_apply_button_clicked (GtkButton *button,
                                                 gpointer user_data)
{
  CheckwriterPreferences *self = NULL;

  self = CHECKWRITER_PREFERENCES (user_data);

  if (!self)
    {
      g_error ("%s: Failed to retrieve instance. Invalid user_data parameter.", __func__);
      return;
    }

  /* Apply the settings */

  if (check_properties_store (&self->check_properties) < 0)
    {
      g_error ("%s: Error changing settings", __func__);
      g_abort ();
    }

  g_debug ("%s: Preference window closed after settings applied", __func__);
  gtk_window_destroy (GTK_WINDOW (self->preferences_window));
}

static void
checkwriter_preferences_on_spin_value_change (GtkSpinButton *spin,
                                              gpointer user_data)
{
  CheckwriterPreferences *self = NULL;

  self = CHECKWRITER_PREFERENCES (user_data);

  if (!self)
    {
      g_error ("%s: Failed to retrieve instance. Invalid user_data parameter.", __func__);
      return;
    }

  /* Get each ID and populate it with appropirate value */
  if (spin == self->check_width_spin)
    {
      self->check_properties.width = gtk_spin_button_get_value (self->check_width_spin);
    }
  else if (spin == self->check_height_spin)
    {
      self->check_properties.height = gtk_spin_button_get_value (self->check_height_spin);
    }
  else if (spin == self->x_pad_spin)
    {
      self->check_properties.x_pad = gtk_spin_button_get_value (self->x_pad_spin);
    }
  else if (spin == self->y_pad_spin)
    {
      self->check_properties.y_pad = gtk_spin_button_get_value (self->y_pad_spin);
    }
  /* Date */
  else if (spin == self->date_x_spin)
    {
      self->check_properties.date.x_pos = gtk_spin_button_get_value (self->date_x_spin);
    }
  else if (spin == self->date_y_spin)
    {
      self->check_properties.date.y_pos = gtk_spin_button_get_value (self->date_y_spin);
    }
  else if (spin == self->date_width_spin)
    {
      self->check_properties.date.width = gtk_spin_button_get_value (self->date_width_spin);
    }
  /* Name */
  else if (spin == self->pay_to_x_spin)
    {
      self->check_properties.name.x_pos = gtk_spin_button_get_value (self->pay_to_x_spin);
    }
  else if (spin == self->pay_to_y_spin)
    {
      self->check_properties.name.y_pos = gtk_spin_button_get_value (self->pay_to_y_spin);
    }
  else if (spin == self->pay_to_width_spin)
    {
      self->check_properties.name.width = gtk_spin_button_get_value (self->pay_to_width_spin);
    }
  /* Amount */
  else if (spin == self->amount_x_spin)
    {
      self->check_properties.amount.x_pos = gtk_spin_button_get_value (self->amount_x_spin);
    }
  else if (spin == self->amount_y_spin)
    {
      self->check_properties.amount.y_pos = gtk_spin_button_get_value (self->amount_y_spin);
    }
  else if (spin == self->amount_width_spin)
    {
      self->check_properties.amount.width = gtk_spin_button_get_value (self->amount_width_spin);
    }
  /* Amount words */
  else if (spin == self->amount_words_x_spin)
    {
      self->check_properties.amount_in_words.x_pos = gtk_spin_button_get_value (self->amount_words_x_spin);
    }
  else if (spin == self->amount_words_y_spin)
    {
      self->check_properties.amount_in_words.y_pos = gtk_spin_button_get_value (self->amount_words_y_spin);
    }
  else if (spin == self->amount_words_width_spin)
    {
      self->check_properties.amount_in_words.width = gtk_spin_button_get_value (self->amount_words_width_spin);
    }
  /* Memo */
  else if (spin == self->memo_x_spin)
    {
      self->check_properties.memo.x_pos = gtk_spin_button_get_value (self->memo_x_spin);
    }
  else if (spin == self->memo_y_spin)
    {
      self->check_properties.memo.y_pos = gtk_spin_button_get_value (self->memo_y_spin);
    }
  else if (spin == self->memo_width_spin)
    {
      self->check_properties.memo.width = gtk_spin_button_get_value (self->memo_width_spin);
    }

  /* Mark global variable changed */
  check_properties_mark_settings_changed ();

  /* Update the check preview */
  if (self->check_preview_area)
    {
      gtk_widget_queue_draw (self->check_preview_area);
    }
}

/**
 * Drawing functions
 */

static void
checkwriter_preferences_draw_check_preview (GtkDrawingArea *area,
                                            cairo_t *cr,
                                            int width,
                                            int height,
                                            gpointer user_data)
{
  CheckwriterPreferences *self = NULL;
  cairo_surface_t *surface = NULL;

  DisplayProperties display;
  CheckData check_data;

  double x_scale, y_scale, x_dpi, y_dpi;

  (void) area;

  self = CHECKWRITER_PREFERENCES (user_data);
  if (!self)
    {
      g_error ("%s: Failed to retrieve instance. Invalid `user_data` parameter.", __func__);
      return;
    }

  /* Get the target surface */
  surface = cairo_get_target (cr);
  if (!surface)
    {
      g_error ("%s: Failed to retrieve cairo instance. Invalid `cr` parameter.", __func__);
      return;
    }

  /* Get the device scale (this accounts for any scaling on HiDPI displays) */
  cairo_surface_get_device_scale (surface, &x_scale, &y_scale);

  x_dpi = DEFAULT_DPI * x_scale;
  y_dpi = DEFAULT_DPI * y_scale;

  display.width = width;
  display.height = height;
  display.x_dpi = x_dpi;
  display.y_dpi = y_dpi;

  check_data_set_sample (&check_data);

  render_check (cr, &display, &self->check_properties, &check_data, CHECK_PREVIEW_ONLY);
}

/**
 * Object Initialization
 */

static void
checkwriter_preferences_class_init (CheckwriterPreferencesClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, CHECKWRITER_PREFERENCES_RESOURCE_FILE);
  /* Bind the variables to the template */

  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, preferences_window);

  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, apply_button);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, cancel_button);

  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, check_width_spin);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, check_height_spin);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, x_pad_spin);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, y_pad_spin);

  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, date_x_spin);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, date_y_spin);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, date_width_spin);

  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, pay_to_x_spin);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, pay_to_y_spin);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, pay_to_width_spin);

  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, amount_x_spin);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, amount_y_spin);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, amount_width_spin);

  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, amount_words_x_spin);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, amount_words_y_spin);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, amount_words_width_spin);

  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, memo_x_spin);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, memo_y_spin);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, memo_width_spin);

  gtk_widget_class_bind_template_child (widget_class, CheckwriterPreferences, check_preview_area);
}

static void
checkwriter_preferences_init (CheckwriterPreferences *self)
{
  /* Initialize template */
  gtk_widget_init_template (GTK_WIDGET (self));

  /* Connect signals */
  g_signal_connect (self->cancel_button, "clicked",
                    G_CALLBACK (checkwriter_preferences_on_cancel_button_clicked), self);

  g_signal_connect (self->apply_button, "clicked",
                    G_CALLBACK (checkwriter_preferences_on_apply_button_clicked), self);

  g_signal_connect (self->check_width_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->check_height_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->x_pad_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->y_pad_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->date_x_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->date_y_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->date_width_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->pay_to_x_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->pay_to_y_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->pay_to_width_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->amount_x_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->amount_y_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->amount_width_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->amount_words_x_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->amount_words_y_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->amount_words_width_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->memo_x_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->memo_y_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  g_signal_connect (self->memo_width_spin, "value-changed",
                    G_CALLBACK (checkwriter_preferences_on_spin_value_change), self);

  /* Connect drawing area update function */
  gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (self->check_preview_area),
                                  checkwriter_preferences_draw_check_preview, self, NULL);

  g_debug ("%s: Init finished", __func__);
}

GtkWindow *
checkwriter_preferences_window_new (GtkApplication *app)
{
  CheckwriterPreferences *prefs = NULL;

  /* Allocate the memory for preference*/
  prefs = g_object_new (CHECKWRITER_TYPE_PREFERENCES, NULL);

  if (!prefs)
    {
      g_error ("Error allocating memory for CheckWriterPreferences");
      return NULL;
    }

  /* Need to load settings manually for some reason */
  checkwriter_preferences_load_settings (prefs);

  return prefs->preferences_window;
}
