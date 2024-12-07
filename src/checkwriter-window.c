/* checkwriter-window.c
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

#include "checkwriter-window.h"

#include "check-properties.h"

#include <ctype.h>

struct _CheckwriterWindow
{
  AdwApplicationWindow parent_instance;

  /* Application state */
  GtkWidget *check_date_calendar;
  GtkWidget *pay_to_order_entry;
  GtkWidget *check_amount_entry;
  GtkWidget *check_memo_entry;

  GtkWidget *check_preview_area;

  GtkWidget *place_on_check_button;
  GtkWidget *print_template_button;

  CheckProperties check_properties;
  CheckData check_data;
};

G_DEFINE_FINAL_TYPE (CheckwriterWindow, checkwriter_window, ADW_TYPE_APPLICATION_WINDOW)

/**
 * Event handlers
 */

static void
checkwriter_window_on_entry_changed (GtkEntry *entry, gpointer user_data)
{
  CheckwriterWindow *window = CHECKWRITER_WINDOW (user_data);
  const gchar *text = gtk_editable_get_text (GTK_EDITABLE (entry));

  if (!window)
    {
      g_debug ("Window not allocated\n");
      return;
    }

  if (!text)
    {
      g_debug ("No text allocated");
      return;
    }

  if (entry == GTK_ENTRY (window->pay_to_order_entry))
    {
      g_strlcpy (window->check_data.name, text, STRING_LEN);
      g_debug ("Pay to the order changed: %s\n", text);
    }
  else if (entry == GTK_ENTRY (window->check_amount_entry))
    {
      /* Attempt to parse the dollar and cents value from entry */
      guint dollars = 0, cents = 0;
      gint written = 0;

      sscanf (text, "%'u.%u", &dollars, &cents);

      /* Write the parsed dollar amount */
      g_snprintf (window->check_data.amount, STRING_LEN, "%'u.%02u", dollars, cents);

      /* Write the amount in words */
      do
        {
          gchar *dst = window->check_data.amount_in_words;
          written = num_to_words (dst, STRING_LEN, dollars);
          dst[0] = toupper (dst[0]);

          g_assert (written >= 0);

          /* Write centes */
          g_snprintf (dst + written, STRING_LEN - written, " and %02u/100", cents);
        }
      while (0);

      g_debug ("Amount changed: %s\n", window->check_data.amount);
    }
  else if (entry == GTK_ENTRY (window->check_memo_entry))
    {
      g_strlcpy (window->check_data.memo, text, STRING_LEN);
      g_debug ("Memo changed: %s\n", text);
    }

  if (window->check_preview_area)
    {
      gtk_widget_queue_draw (window->check_preview_area);
    }
}

static void
checkwriter_window_on_calendar_shown (GtkCalendar *self, gpointer user_data)
{
  CheckwriterWindow *window = CHECKWRITER_WINDOW (user_data);
  GDateTime *now = NULL;

  if (self == GTK_CALENDAR (window->check_date_calendar))
    {
      g_signal_emit_by_name (self, "day-selected");

      now = g_date_time_new_now_local ();
      gtk_calendar_select_day (self, now);
      g_date_time_unref (now);
    }
}

static void
checkwriter_window_on_day_selected (GtkCalendar *self, gpointer user_data)
{
  CheckwriterWindow *window = CHECKWRITER_WINDOW (user_data);

  if (self == GTK_CALENDAR (window->check_date_calendar))
    {
      int day = gtk_calendar_get_day (self);
      int month = (1 + gtk_calendar_get_month (self));
      int year = gtk_calendar_get_year (self);

      g_snprintf (window->check_data.date, STRING_LEN, "%02d/%02d/%04d", month, day, year);
      g_debug ("Date changed: %02d/%02d/%04d\n", month, day, year);
    }

  if (window->check_preview_area)
    {
      gtk_widget_queue_draw (window->check_preview_area);
    }
}

static void
checkwriter_window_draw_check_preview (GtkDrawingArea *area,
                                       cairo_t *cr,
                                       int width,
                                       int height,
                                       gpointer user_data)
{
  CheckwriterWindow *window = NULL;
  cairo_surface_t *surface = NULL;
  double x_scale, y_scale, x_dpi, y_dpi;
  DisplayProperties display;
  CheckProperties *check_properties = NULL;
  CheckData *check_data = NULL;

  (void) area;

  window = CHECKWRITER_WINDOW (user_data);

  /* Get the target surface */
  surface = cairo_get_target (cr);

  /* Get the device scale (this accounts for any scaling on HiDPI displays) */
  cairo_surface_get_device_scale (surface, &x_scale, &y_scale);

  x_dpi = DEFAULT_DPI * x_scale;
  y_dpi = DEFAULT_DPI * y_scale;

  display.width = width;
  display.height = height;
  display.x_dpi = x_dpi;
  display.y_dpi = y_dpi;

  check_properties = &window->check_properties;
  check_data = &window->check_data;

  render_check (cr, &display, check_properties, check_data, CHECK_PREVIEW_ONLY);
}

/**
 * Printing functions (Regular, Fill check)
 */

static void
checkwrter_window_on_draw_page (GtkPrintOperation *operation,
                                GtkPrintContext *context,
                                int page_nr,
                                gpointer user_data)
{
  CheckwriterWindow *window = NULL;
  cairo_t *cr = NULL;
  double x_dpi, y_dpi, width, height;
  DisplayProperties display;
  CheckProperties *check_properties = NULL;
  CheckData *check_data = NULL;

  (void) page_nr;
  (void) operation;

  window = CHECKWRITER_WINDOW (user_data);

  cr = gtk_print_context_get_cairo_context (context);

  /* Get DPI for X and Y axes from the print context */
  x_dpi = gtk_print_context_get_dpi_x (context);
  y_dpi = gtk_print_context_get_dpi_y (context);
  width = gtk_print_context_get_width (context);
  height = gtk_print_context_get_height (context);

  display.width = width;
  display.height = height;
  display.x_dpi = x_dpi;
  display.y_dpi = y_dpi;

  check_properties = &window->check_properties;
  check_data = &window->check_data;

  render_check (cr, &display, check_properties, check_data, CHECK_WRITE);

  g_debug ("Done rendering page\n");
}

static void
checkwriter_window_on_begin_print (GtkPrintOperation *operation,
                                   GtkPrintContext *context,
                                   gpointer user_data)
{
  // Unused variables
  (void) context;
  (void) user_data;

  // Inform the print operation how many pages to expect.
  gtk_print_operation_set_n_pages (operation, 1);
  g_debug ("Print operation begins\n");
}

static void
checkwriter_window_on_print_check_clicked (GtkWidget *button,
                                           gpointer user_data)
{
  CheckwriterWindow *window = NULL;

  (void) button;
  window = CHECKWRITER_WINDOW (user_data);

  GtkPrintOperation *print = gtk_print_operation_new ();

  g_signal_connect (print, "begin_print", G_CALLBACK (checkwriter_window_on_begin_print), user_data);
  g_signal_connect (print, "draw_page", G_CALLBACK (checkwrter_window_on_draw_page), user_data);

  GtkPrintOperationResult res = gtk_print_operation_run (
      print, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW (window), NULL);

  if (res == GTK_PRINT_OPERATION_RESULT_ERROR)
    {
      g_debug ("Error in print operation\n");
    }

  g_object_unref (print);
}

/**
 * Printing functions (Regular, Fill check)
 */

static char *
setcharx (char *dst, int ch, size_t count)
{
  dst[count] = '\0'; /* Properly NULL terminate the string */
  return memset (dst, ch, count);
}

static void
checkwrter_window_on_draw_template_page (GtkPrintOperation *operation,
                                         GtkPrintContext *context,
                                         int page_nr,
                                         gpointer user_data)
{
  CheckwriterWindow *window = NULL;
  cairo_t *cr = NULL;
  double x_dpi, y_dpi, width, height;
  DisplayProperties display;
  CheckProperties *check_properties = NULL;
  CheckData check_data;

  (void) page_nr;
  (void) operation;

  window = CHECKWRITER_WINDOW (user_data);

  cr = gtk_print_context_get_cairo_context (context);

  /* Get DPI for X and Y axes from the print context */
  x_dpi = gtk_print_context_get_dpi_x (context);
  y_dpi = gtk_print_context_get_dpi_y (context);
  width = gtk_print_context_get_width (context);
  height = gtk_print_context_get_height (context);

  display.width = width;
  display.height = height;
  display.x_dpi = x_dpi;
  display.y_dpi = y_dpi;

  check_properties = &window->check_properties;

  /* TODO: Remove hard coded constants and fill them with pattern instead  */
  snprintf (check_data.date, STRING_LEN, "MM/DD/YYYY");
  setcharx (check_data.name, 'X', 44);
  setcharx (check_data.amount, 'X', 10);
  setcharx (check_data.amount_in_words, 'X', 52);
  setcharx (check_data.memo, 'X', 26);

  render_check (cr, &display, check_properties, &check_data, CHECK_TEMPLATE);

  g_debug ("Done rendering page\n");
}

static void
checkwriter_window_on_print_template_clicked (GtkWidget *button,
                                              gpointer user_data)
{
  CheckwriterWindow *window = NULL;

  (void) button;
  window = CHECKWRITER_WINDOW (user_data);

  GtkPrintOperation *print = gtk_print_operation_new ();

  g_signal_connect (print, "begin_print", G_CALLBACK (checkwriter_window_on_begin_print), user_data);
  g_signal_connect (print, "draw_page", G_CALLBACK (checkwrter_window_on_draw_template_page), user_data);

  GtkPrintOperationResult res = gtk_print_operation_run (
      print, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW (window), NULL);

  if (res == GTK_PRINT_OPERATION_RESULT_ERROR)
    {
      g_debug ("Error in print operation\n");
    }

  g_object_unref (print);
}

/**
 * Window and Application Initializations
 */

static void
checkwriter_window_class_init (CheckwriterWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/at/shafq/checkwriter/checkwriter-window.ui");

  /* Bind the variables to the template */
  gtk_widget_class_bind_template_child (widget_class, CheckwriterWindow, check_date_calendar);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterWindow, pay_to_order_entry);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterWindow, check_amount_entry);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterWindow, check_memo_entry);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterWindow, place_on_check_button);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterWindow, print_template_button);
  gtk_widget_class_bind_template_child (widget_class, CheckwriterWindow, check_preview_area);
}

static void
checkwriter_window_init (CheckwriterWindow *self)
{
  /* Initialize template */
  gtk_widget_init_template (GTK_WIDGET (self));

  /* Initialize check properties */
  check_properties_init (&self->check_properties);
  check_data_init (&self->check_data);

  /* Connect calendar "day-selected" signal */
  g_signal_connect (self->check_date_calendar, "day-selected", G_CALLBACK (checkwriter_window_on_day_selected), self);

  /* Force "day-selected" when the calendar is shown */
  g_signal_connect (self->check_date_calendar, "map", G_CALLBACK (checkwriter_window_on_calendar_shown), self);

  /* Connect the text entry "changed" signal for each text entry */
  g_signal_connect (self->pay_to_order_entry, "changed", G_CALLBACK (checkwriter_window_on_entry_changed), self);
  g_signal_connect (self->check_amount_entry, "changed", G_CALLBACK (checkwriter_window_on_entry_changed), self);
  g_signal_connect (self->check_memo_entry, "changed", G_CALLBACK (checkwriter_window_on_entry_changed), self);

  /* Connect drawing area update function */
  gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (self->check_preview_area),
                                  checkwriter_window_draw_check_preview, self, NULL);

  /* Connect button events for printing */
  g_signal_connect (self->place_on_check_button, "clicked", G_CALLBACK (checkwriter_window_on_print_check_clicked),
                    self);

  g_signal_connect (self->print_template_button, "clicked", G_CALLBACK (checkwriter_window_on_print_template_clicked),
                    self);
}
