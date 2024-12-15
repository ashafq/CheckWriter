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

#include "config.h"

#include "check-properties.h"

#define CHECKWRITER_GSETTINGS_URI (PACKAGE_URI)

#include <math.h>
#include <string.h>

#define ENABLE_SCALING(flags) ((flags) & 0x02)
#define ENABLE_LINES(flags) ((flags) & 0x01)
#define PATTERN_SIZE 4 /* Width and height of the tile */

static volatile guint CHECK_PROPERTIES_CHANGED = 0;

static bool
check_properties_initialized (const CheckProperties *p)
{
  if (p)
    {
      return p->magic == CHECK_PROPERTIES_MAGIC;
    }
  else
    {
      return false;
    }
}

/* Load configuration from GSettings */
int
check_properties_load (CheckProperties *p)
{
  if (!p)
    {
      return -1;
    }

  GSettings *settings = NULL;

  settings = g_settings_new (CHECKWRITER_GSETTINGS_URI);

  if (!settings)
    {
      return -2;
    }

  /* General properties */
  g_strlcpy (p->check_font, g_settings_get_string (settings, "check-font"), STRING_LEN);
  p->check_font_height = g_settings_get_int (settings, "check-font-height");

  p->width = g_settings_get_double (settings, "check-width-mm");
  p->height = g_settings_get_double (settings, "check-height-mm");
  p->x_pad = g_settings_get_double (settings, "check-pad-x-mm");
  p->y_pad = g_settings_get_double (settings, "check-pad-y-mm");

  /* Date properties */
  p->date.x_pos = g_settings_get_double (settings, "check-date-pos-x-mm");
  p->date.y_pos = g_settings_get_double (settings, "check-date-pos-y-mm");
  p->date.width = g_settings_get_double (settings, "check-date-width-mm");

  /* Name properties */
  p->name.x_pos = g_settings_get_double (settings, "check-name-pos-x-mm");
  p->name.y_pos = g_settings_get_double (settings, "check-name-pos-y-mm");
  p->name.width = g_settings_get_double (settings, "check-name-width-mm");

  /* Amount properties */
  p->amount.x_pos = g_settings_get_double (settings, "check-amount-pos-x-mm");
  p->amount.y_pos = g_settings_get_double (settings, "check-amount-pos-y-mm");
  p->amount.width = g_settings_get_double (settings, "check-amount-width-mm");

  /* Amount in words properties */
  p->amount_in_words.x_pos = g_settings_get_double (settings, "check-amount-in-words-pos-x-mm");
  p->amount_in_words.y_pos = g_settings_get_double (settings, "check-amount-in-words-pos-y-mm");
  p->amount_in_words.width = g_settings_get_double (settings, "check-amount-in-words-width-mm");

  /* Memo properties */
  p->memo.x_pos = g_settings_get_double (settings, "check-memo-pos-x-mm");
  p->memo.y_pos = g_settings_get_double (settings, "check-memo-pos-y-mm");
  p->memo.width = g_settings_get_double (settings, "check-memo-width-mm");

  p->magic = CHECK_PROPERTIES_MAGIC;

  /* Free GSettings object */
  g_object_unref (settings);

  /* Mark reload to false */
  CHECK_PROPERTIES_CHANGED = false;

  return 0;
}

void
check_properties_mark_settings_changed (void)
{
  g_debug ("%s: Marking properties changed", __func__);

  g_atomic_int_set (&CHECK_PROPERTIES_CHANGED, 1);
}

bool
check_properties_settings_changed (void)
{
  guint status = g_atomic_int_get (&CHECK_PROPERTIES_CHANGED);

  g_debug ("%s: status = %u", __func__, status);

  return status != 0;
}

int
check_properties_store (CheckProperties *p)
{
  if (!p)
    {
      return -1;
    }

  GSettings *settings = NULL;

  settings = g_settings_new (CHECKWRITER_GSETTINGS_URI);

  if (!settings)
    {
      return -2;
    }

#define g_settings_set_double_chk(settings, key, value)  \
  do                                                     \
    {                                                    \
      if (!g_settings_set_double (settings, key, value)) \
        {                                                \
          return -3;                                     \
        }                                                \
    }                                                    \
  while (0)

  g_settings_set_double_chk (settings, "check-width-mm", p->width);
  g_settings_set_double_chk (settings, "check-height-mm", p->height);
  g_settings_set_double_chk (settings, "check-pad-x-mm", p->x_pad);
  g_settings_set_double_chk (settings, "check-pad-y-mm", p->y_pad);

  /* Date properties */
  g_settings_set_double_chk (settings, "check-date-pos-x-mm", p->date.x_pos);
  g_settings_set_double_chk (settings, "check-date-pos-y-mm", p->date.y_pos);
  g_settings_set_double_chk (settings, "check-date-width-mm", p->date.width);

  /* Name properties */
  g_settings_set_double_chk (settings, "check-name-pos-x-mm", p->name.x_pos);
  g_settings_set_double_chk (settings, "check-name-pos-y-mm", p->name.y_pos);
  g_settings_set_double_chk (settings, "check-name-width-mm", p->name.width);

  /* Amount properties */
  g_settings_set_double_chk (settings, "check-amount-pos-x-mm", p->amount.x_pos);
  g_settings_set_double_chk (settings, "check-amount-pos-y-mm", p->amount.y_pos);
  g_settings_set_double_chk (settings, "check-amount-width-mm", p->amount.width);

  /* Amount in words properties */
  g_settings_set_double_chk (settings, "check-amount-in-words-pos-x-mm", p->amount_in_words.x_pos);
  g_settings_set_double_chk (settings, "check-amount-in-words-pos-y-mm", p->amount_in_words.y_pos);
  g_settings_set_double_chk (settings, "check-amount-in-words-width-mm", p->amount_in_words.width);

  /* Memo properties */
  g_settings_set_double_chk (settings, "check-memo-pos-x-mm", p->memo.x_pos);
  g_settings_set_double_chk (settings, "check-memo-pos-y-mm", p->memo.y_pos);
  g_settings_set_double_chk (settings, "check-memo-width-mm", p->memo.width);

#undef g_settings_set_double_chk
  /* Free GSettings object */
  g_object_unref (settings);
  return 0;
}

void
check_data_init (CheckData *p)
{
  if (p)
    {
      memset (p, 0, sizeof (CheckData));
    }
}

static inline char *
setcharx (char *dst, int ch, size_t count)
{
  dst[count] = '\0'; /* Properly NULL terminate the string */
  return memset (dst, ch, count);
}

void
check_data_set_sample (CheckData *check_data)
{
  if (check_data)
    {
      /* TODO: Remove hard coded constants and fill them with pattern instead  */
      snprintf (check_data->date, STRING_LEN, "MM/DD/YYYY");
      setcharx (check_data->name, 'X', 44);
      setcharx (check_data->amount, 'X', 10);
      setcharx (check_data->amount_in_words, 'X', 52);
      setcharx (check_data->memo, 'X', 26);
    }
}

static double
mm_to_px (double mm, double dpi)
{
  return (mm * dpi) / INCH_PER_MM;
}

static double
pts_to_px (double pts, double dpi)
{
  return (pts * dpi) / POINTS_PER_INCH;
}

void
render_check (cairo_t *cr,
              const DisplayProperties *display_prop,
              const CheckProperties *check_prop,
              const CheckData *check_data,
              int flags)
{
  if (!check_properties_initialized (check_prop))
    {
      return;
    }

  const double width = display_prop->width;
  const double height = display_prop->height;
  const double x_dpi = display_prop->x_dpi;
  const double y_dpi = display_prop->y_dpi;
  const double x_pad = mm_to_px (check_prop->x_pad, x_dpi);
  const double y_pad = mm_to_px (check_prop->y_pad, y_dpi);

  const char *check_font = check_prop->check_font;
  const int check_font_height = check_prop->check_font_height;

  /* Convert check dimensions from mm to pixels */
  const double check_width_px = mm_to_px (check_prop->width, x_dpi);
  const double check_height_px = mm_to_px (check_prop->height, y_dpi);

  const char *date = check_data->date;
  const char *name = check_data->name;
  const char *amount = check_data->amount;
  const char *amount_in_words = check_data->amount_in_words;
  const char *memo = check_data->memo;

  double scale = 1.0, scale_x = 1.0, scale_y = 1.0;
  /* Calculate offsets to center the check */
  double x_offset = fmax ((width - check_width_px) / 2.0, 0.0);
  double y_offset = fmax ((height - check_height_px) / 2.0, 0.0);

  /* Apply scaling to the canvas */
  if (ENABLE_SCALING (flags))
    {
      /* Calculate scale factors for X and Y axes based on the available width and */
      /* height */
      scale_x = width / check_width_px;
      scale_y = height / check_height_px;
      scale = fmin (scale_x, scale_y);
      x_offset = 0;
      y_offset = 0;
      cairo_scale (cr, scale, scale);
    }

  /* Set the background color to white and fill the area */
  cairo_set_source_rgb (cr, 1, 1, 1); /* White background */
  cairo_paint (cr);

  /* Draw tiled greyscale pattern from an array */
  if (ENABLE_LINES (flags))
    {
      /* Define a small greyscale pattern (1 byte per pixel) */
      static uint8_t pattern_data[PATTERN_SIZE * PATTERN_SIZE] = {
        0x20, 0x00, 0x00, 0x10,
        0x00, 0x20, 0x10, 0x00,
        0x00, 0x10, 0x20, 0x00,
        0x10, 0x00, 0x00, 0x20
      };

      /* Create a surface for the greyscale pattern */
      cairo_surface_t *pattern_surface = cairo_image_surface_create_for_data (
          pattern_data,
          CAIRO_FORMAT_A8, /* Grayscale format */
          PATTERN_SIZE,
          PATTERN_SIZE,
          PATTERN_SIZE /* Stride (bytes per row) */
      );

      /* Create a Cairo pattern from the surface */
      cairo_pattern_t *pattern = cairo_pattern_create_for_surface (pattern_surface);
      cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);

      /* Fill the check area with the tiled pattern */
      cairo_set_source (cr, pattern);
      cairo_rectangle (cr, x_offset, y_offset, check_width_px, check_height_px);
      cairo_fill (cr);

      /* Cleanup */
      cairo_pattern_destroy (pattern);
      cairo_surface_destroy (pattern_surface);

      /* Reset the color for subsequent drawing */
      cairo_set_source_rgb (cr, 0, 0, 0);
    }

  /* Draw the check border rectangle with padding */
  if (ENABLE_LINES (flags))
    {
      cairo_set_source_rgb (cr, 1, 0, 0); /* Red border */
      cairo_rectangle (cr, x_offset, y_offset, check_width_px, check_height_px);
      cairo_set_line_width (cr, 1);
      cairo_stroke (cr); /* Draw the border */
    }

  /* Draw Date */
  double date_x = mm_to_px (check_prop->date.x_pos, x_dpi);
  double date_y = mm_to_px (check_prop->date.y_pos, y_dpi);

  if (date)
    {
      cairo_set_source_rgb (cr, 0, 0, 0); /* Black text */
      cairo_select_font_face (cr, check_font, CAIRO_FONT_SLANT_NORMAL,
                              CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cr, check_font_height);

      cairo_text_extents_t extents;
      cairo_text_extents (cr, date, &extents);
      cairo_move_to (cr, x_offset + x_pad + date_x + (extents.width / 2.0),
                     y_offset + date_y - y_pad);
      cairo_show_text (cr, date);
    }

  /* Draw underline for date */
  if (ENABLE_LINES (flags))
    {
      cairo_set_source_rgb (cr, 0, 0, 1);
      double date_width_px = mm_to_px (check_prop->date.width, x_dpi);
      cairo_move_to (cr, x_offset + date_x, y_offset + date_y);
      cairo_line_to (cr, x_offset + date_x + date_width_px, y_offset + date_y);
      cairo_stroke (cr);

      /* Set font and text color for the label */
      cairo_set_source_rgb (cr, 0, 0, 0);
      cairo_select_font_face (cr, CHECK_VIEW_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cr, CHECK_VIEW_FONT_HEIGHT);

      /* Calculate the position for "Date" so it ends before the underline */
      cairo_text_extents_t text_extents;
      cairo_text_extents (cr, "Date", &text_extents);

      /* Calculate the starting X position for "Date" */
      double text_x = x_offset + date_x - x_pad - text_extents.width;
      double text_y = y_offset + date_y - y_pad;

      /* Draw "Date" */
      cairo_move_to (cr, text_x, text_y);
      cairo_show_text (cr, "Date");
    }

  /* Draw Name */
  double name_x = mm_to_px (check_prop->name.x_pos, x_dpi);
  double name_y = mm_to_px (check_prop->name.y_pos, y_dpi);

  if (name)
    {
      /* Calculate text offset */
      double text_start_x = x_offset + name_x + x_pad;
      double text_start_y = y_offset + name_y - y_pad;

      cairo_set_source_rgb (cr, 0, 0, 0); /* Black text */
      cairo_select_font_face (cr, check_font, CAIRO_FONT_SLANT_NORMAL,
                              CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cr, check_font_height);

      cairo_move_to (cr, text_start_x, text_start_y);
      cairo_show_text (cr, name);
    }

  /* Draw underline for name */
  if (ENABLE_LINES (flags))
    {
      double name_width_px = mm_to_px (check_prop->name.width, x_dpi);

      double line_start_x = x_offset + name_x;
      double line_end_x = line_start_x + name_width_px;
      double line_y = y_offset + name_y;

      cairo_set_source_rgb (cr, 0, 0, 1);
      cairo_move_to (cr, line_start_x, line_y);
      cairo_line_to (cr, line_end_x, line_y);
      cairo_stroke (cr);

      /* Set font and text color for the label */
      cairo_set_source_rgb (cr, 0, 0, 0);
      cairo_select_font_face (cr, CHECK_VIEW_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cr, CHECK_VIEW_FONT_HEIGHT);

      /* Calculate the position for "Pay To" so it ends before the underline */
      cairo_text_extents_t text_extents;
      cairo_text_extents (cr, "Pay To", &text_extents);

      /* Calculate the starting X position for "Pay To" */
      double text_x = x_offset + name_x - x_pad - text_extents.width;
      double text_y = y_offset + name_y - y_pad;

      /* Draw "Pay To" */
      cairo_move_to (cr, text_x, text_y);
      cairo_show_text (cr, "Pay To");
    }

  /* Draw Amount */
  double amount_x = mm_to_px (check_prop->amount.x_pos, x_dpi);
  double amount_y = mm_to_px (check_prop->amount.y_pos, y_dpi);

  if (amount)
    {
      cairo_set_source_rgb (cr, 0, 0, 0); /* Black text */
      cairo_select_font_face (cr, check_font, CAIRO_FONT_SLANT_NORMAL,
                              CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cr, check_font_height);

      cairo_move_to (cr, x_offset + amount_x + x_pad, y_offset + amount_y - y_pad);
      cairo_show_text (cr, amount);
    }

  /* Draw underline for amount */
  if (ENABLE_LINES (flags))
    {
      double amount_width_px = mm_to_px (check_prop->amount.width, x_dpi);
      double line_start_x = x_offset + amount_x;
      double line_end_x = line_start_x + amount_width_px;
      double line_y = y_offset + amount_y;

      cairo_set_source_rgb (cr, 0, 0, 1);
      cairo_move_to (cr, line_start_x, line_y);
      cairo_line_to (cr, line_end_x, line_y);
      cairo_stroke (cr);

      /* Set font and text color for the label */
      cairo_set_source_rgb (cr, 0, 0, 0);
      cairo_select_font_face (cr, CHECK_VIEW_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cr, CHECK_VIEW_FONT_HEIGHT);

      /* Calculate the position for "$" so it ends before the underline */
      cairo_text_extents_t text_extents;
      cairo_text_extents (cr, "$", &text_extents);

      /* Calculate the starting X position for "$" */
      double text_x = x_offset + amount_x - x_pad - text_extents.width;
      double text_y = y_offset + amount_y - y_pad;

      /* Draw "$" */
      cairo_move_to (cr, text_x, text_y);
      cairo_show_text (cr, "$");
    }

  /* Draw Amount in Words */
  double amount_words_x = mm_to_px (check_prop->amount_in_words.x_pos, x_dpi);
  double amount_words_y = mm_to_px (check_prop->amount_in_words.y_pos, y_dpi);
  double amount_words_width_px = mm_to_px (check_prop->amount_in_words.width, x_dpi);

  if (strlen (amount_in_words) > 0)
    {
      /* Calculate text offset */
      double text_start_x = x_offset + amount_words_x + x_pad;
      double text_start_y = y_offset + amount_words_y - y_pad;
      cairo_text_extents_t extents;

      cairo_set_source_rgb (cr, 0, 0, 0); /* Black text */
      cairo_select_font_face (cr, check_font, CAIRO_FONT_SLANT_NORMAL,
                              CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cr, check_font_height);
      cairo_text_extents (cr, amount_in_words, &extents);

      cairo_move_to (cr, text_start_x, text_start_y);
      cairo_show_text (cr, amount_in_words);

      /* Calculate where the dotted line should start (aligned with the end of the text) */
      double line_start_x = text_start_x + extents.width + x_pad;
      double line_end_x = line_start_x + (amount_words_width_px - extents.width) - (2 * x_pad);
      double line_y = text_start_y - (pts_to_px (check_font_height, y_dpi) / 2.0) + y_pad;

      /* Only draw this line if within the border */
      if (line_end_x > line_start_x)
        {
          /* Set the dash pattern for dotted line (5 pixels on, 3 pixels off) */
          double dashes[] = { 3.0, 3.0 };
          cairo_set_dash (cr, dashes, 2, 0); /* Set dash pattern */

          /* Draw the dotted line */
          cairo_move_to (cr, line_start_x, line_y);
          cairo_line_to (cr, line_end_x, line_y);
          cairo_stroke (cr); /* Render the dotted line */

          /* Reset the dash pattern to solid line for future strokes */
          cairo_set_dash (cr, NULL, 0, 0);
        }
    }

  /* Draw underline for amount in words */
  if (ENABLE_LINES (flags))
    {
      double line_start_x = x_offset + amount_words_x;
      double line_end_x = line_start_x + amount_words_width_px;
      double line_y = y_offset + amount_words_y;

      cairo_set_source_rgb (cr, 0, 0, 1);
      cairo_move_to (cr, line_start_x, line_y);
      cairo_line_to (cr, line_end_x, line_y);
      cairo_stroke (cr);

      /* Set font and text color for the label */
      cairo_set_source_rgb (cr, 0, 0, 0);
      cairo_select_font_face (cr, CHECK_VIEW_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cr, CHECK_VIEW_FONT_HEIGHT);

      cairo_move_to (cr, line_end_x + x_pad, line_y - y_pad);
      cairo_show_text (cr, "Dollars");
    }

  /* Draw Memo */
  double memo_x = mm_to_px (check_prop->memo.x_pos, x_dpi);
  double memo_y = mm_to_px (check_prop->memo.y_pos, y_dpi);

  if (memo)
    {
      double text_start_x = x_offset + memo_x + x_pad;
      double text_start_y = y_offset + memo_y - y_pad;
      cairo_set_source_rgb (cr, 0, 0, 0); /* Black text */
      cairo_select_font_face (cr, check_font, CAIRO_FONT_SLANT_NORMAL,
                              CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cr, check_font_height);
      cairo_move_to (cr, text_start_x, text_start_y);
      cairo_show_text (cr, memo);
    }

  /* Draw underline for memo */
  if (ENABLE_LINES (flags))
    {
      double memo_width_px = mm_to_px (check_prop->memo.width, x_dpi);
      double line_start_x = x_offset + memo_x;
      double line_end_x = line_start_x + memo_width_px;
      double line_y = y_offset + memo_y;

      cairo_set_source_rgb (cr, 0, 0, 1);
      cairo_move_to (cr, line_start_x, line_y);
      cairo_line_to (cr, line_end_x, line_y);
      cairo_stroke (cr);
      cairo_set_source_rgb (cr, 0, 0, 0);

      /* Set font and text color for the label */
      cairo_set_source_rgb (cr, 0, 0, 0);
      cairo_select_font_face (cr, CHECK_VIEW_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
      cairo_set_font_size (cr, CHECK_VIEW_FONT_HEIGHT - 2);

      /* Calculate the position for "Memo" so it ends before the underline */
      cairo_text_extents_t text_extents;
      cairo_text_extents (cr, "Memo", &text_extents);

      /* Calculate the starting X position for "Memo" */
      double text_x = x_offset + memo_x - x_pad - text_extents.width;
      double text_y = y_offset + memo_y - y_pad;

      /* Draw "Memo" */
      cairo_move_to (cr, text_x, text_y);
      cairo_show_text (cr, "Memo");
    }
}
