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

#include "check-properties.h"

#include <math.h>
#include <string.h>

#define ENABLE_SCALING(flags) ((flags) & 0x02)
#define ENABLE_LINES(flags) ((flags) & 0x01)

/* FIXME: Load from a config file */
void
check_properties_init (CheckProperties *p)
{
  if (!p)
    {
      return;
    }

  p->width = CHECK_WIDTH_MM;
  p->height = CHECK_HEIGHT_MM;
  p->x_pad = CHECK_PADX_MM;
  p->y_pad = CHECK_PADY_MM;

  p->date.x_pos = CHECK_DATE_POSX_MM;
  p->date.y_pos = CHECK_DATE_POSY_MM;
  p->date.width = CHECK_DATE_WIDTH_MM;

  p->name.x_pos = CHECK_NAME_POSX_MM;
  p->name.y_pos = CHECK_NAME_POSY_MM;
  p->name.width = CHECK_NAME_WIDTH_MM;

  p->amount.x_pos = CHECK_AMOUNT_POSX_MM;
  p->amount.y_pos = CHECK_AMOUNT_POSY_MM;
  p->amount.width = CHECK_AMOUNT_WIDTH_MM;

  p->amount_in_words.x_pos = CHECK_AMOUNT_IN_WORDS_POSX_MM;
  p->amount_in_words.y_pos = CHECK_AMOUNT_IN_WORDS_POSY_MM;
  p->amount_in_words.width = CHECK_AMOUNT_IN_WORDS_WIDTH_MM;

  p->memo.x_pos = CHECK_MEMO_POSX_MM;
  p->memo.y_pos = CHECK_MEMO_POSY_MM;
  p->memo.width = CHECK_MEMO_WIDTH_MM;
}

void
check_data_init (CheckData *p)
{
  if (p)
    {
      memset (p, 0, sizeof (CheckData));
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
  const double width = display_prop->width;
  const double height = display_prop->height;
  const double x_dpi = display_prop->x_dpi;
  const double y_dpi = display_prop->y_dpi;
  const double x_pad = mm_to_px (check_prop->x_pad, x_dpi);
  const double y_pad = mm_to_px (check_prop->y_pad, y_dpi);

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

  /* Draw the check border rectangle with padding */
  if (ENABLE_LINES (flags))
    {
      cairo_set_source_rgb (cr, 1, 0, 0);
      cairo_rectangle (cr, x_offset, y_offset, check_width_px, check_height_px);
      cairo_set_line_width (cr, 2);
      cairo_stroke (cr);
      cairo_set_source_rgb (cr, 0, 0, 0);
      cairo_set_line_width (cr, 1);
    }

  /* Set font and text color */
  cairo_set_source_rgb (cr, 0, 0, 0); /* Black text */
  cairo_select_font_face (cr, CHECK_FONT, CAIRO_FONT_SLANT_NORMAL,
                          CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (cr, CHECK_FONT_HEIGHT);

  /* Draw Date */
  double date_x = mm_to_px (check_prop->date.x_pos, x_dpi);
  double date_y = mm_to_px (check_prop->date.y_pos, y_dpi);

  if (date)
    {
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
      cairo_set_source_rgb (cr, 0, 0, 0);
    }

  /* Draw Name */
  double name_x = mm_to_px (check_prop->name.x_pos, x_dpi);
  double name_y = mm_to_px (check_prop->name.y_pos, y_dpi);

  if (name)
    {
      /* Calculate text offset */
      double text_start_x = x_offset + name_x + x_pad;
      double text_start_y = y_offset + name_y - y_pad;

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
      cairo_set_source_rgb (cr, 0, 0, 0);
    }

  /* Draw Amount */
  double amount_x = mm_to_px (check_prop->amount.x_pos, x_dpi);
  double amount_y = mm_to_px (check_prop->amount.y_pos, y_dpi);

  if (amount)
    {
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
      cairo_set_source_rgb (cr, 0, 0, 0);
    }

  /* Draw Amount in Words */
  double amount_words_x = mm_to_px (check_prop->amount_in_words.x_pos, x_dpi);
  double amount_words_y = mm_to_px (check_prop->amount_in_words.y_pos, y_dpi);
  double amount_words_width_px = mm_to_px (check_prop->amount_in_words.width, x_dpi);

  if (strlen (amount_in_words) > 0)
    {
      cairo_text_extents_t extents;
      cairo_text_extents (cr, amount_in_words, &extents);

      /* Calculate text offset */
      double text_start_x = x_offset + amount_words_x + x_pad;
      double text_start_y = y_offset + amount_words_y - y_pad;

      cairo_move_to (cr, text_start_x, text_start_y);
      cairo_show_text (cr, amount_in_words);

      /* Calculate where the dotted line should start (after the text) */
      double line_start_x = text_start_x + extents.width + x_pad;
      double line_end_x =
          line_start_x + (amount_words_width_px - extents.width) - (2 * x_pad);
      double line_y =
          text_start_y - (pts_to_px (CHECK_FONT_HEIGHT, y_dpi) / 2.0) + y_pad;

      /* Only draw this line if within the border */
      if (line_end_x > line_start_x)
        {
          /* Set the dash pattern for dotted line (5 pixels on, 3 pixels off) */
          double dashes[] = { 5.0, 3.0 };
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
      cairo_set_source_rgb (cr, 0, 0, 0);
    }

  /* Draw Memo */
  double memo_x = mm_to_px (check_prop->memo.x_pos, x_dpi);
  double memo_y = mm_to_px (check_prop->memo.y_pos, y_dpi);

  if (memo)
    {
      double text_start_x = x_offset + memo_x + x_pad;
      double text_start_y = y_offset + memo_y - y_pad;
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
    }
}
