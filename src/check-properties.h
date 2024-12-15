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

#ifndef CHECKWRITER_CEHCK_PROPERTIES_H_
#define CHECKWRITER_CEHCK_PROPERTIES_H_

#include <cairo.h>
#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>

#include <stddef.h>
#include <stdint.h>

/* Constants */
#define DEFAULT_DPI (96.0)
#define INCH_PER_MM (25.4)
#define POINTS_PER_INCH (72.0)

/* Check view properties */
#define CHECK_VIEW_FONT ("Sans")
#define CHECK_VIEW_FONT_HEIGHT (9)

#define STRING_LEN (256)
#define CHECK_PROPERTIES_MAGIC (0xFE55AACC)

/* Check view flags */
#define CHECK_PREVIEW_ONLY (0x3)
#define CHECK_TEMPLATE (0x1)
#define CHECK_WRITE (0x0)

typedef struct check_field_propperties
{
  double x_pos; /* X position in mm */
  double y_pos; /* Y position in mm */
  double width; /* Width in mm */
} FieldProperties;

/* All fields are in millimeters */
typedef struct check_properties
{
  FieldProperties date;
  FieldProperties name;
  FieldProperties amount;
  FieldProperties amount_in_words;
  FieldProperties memo;

  char check_font[STRING_LEN]; /* Fonts for check fields */
  int check_font_height;       /* Font height in points */

  double width;  /* Width in mm */
  double height; /* Height in mm */
  double x_pad;  /* Horizontal padding in fields */
  double y_pad;  /* Vertical padding */

  uint32_t magic; /* Magic value to signal initialized */
} CheckProperties;

typedef struct check_data
{
  char date[STRING_LEN];
  char name[STRING_LEN];
  char amount[STRING_LEN];
  char amount_in_words[STRING_LEN];
  char memo[STRING_LEN];
} CheckData;

typedef struct display_properties
{
  double width;
  double height;
  double x_dpi;
  double y_dpi;
} DisplayProperties;

int check_properties_load (CheckProperties *p);

void check_properties_mark_settings_changed (void);

bool check_properties_settings_changed (void);

int check_properties_store (CheckProperties *p);

void check_data_init (CheckData *p);

int num_to_words (char *dst,
                  size_t len,
                  uint32_t num);

void render_check (cairo_t *cr,
                   const DisplayProperties *dprop,
                   const CheckProperties *cprop,
                   const CheckData *cdata,
                   int flags);

void check_data_set_sample (CheckData *check_data);

#endif /* CHECKWRITER_CEHCK_PROPERTIES_H_ */
