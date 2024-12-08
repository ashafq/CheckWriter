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

#ifndef CHECKWRITER_CEHCK_PROPERTIES_H_
#define CHECKWRITER_CEHCK_PROPERTIES_H_

#include <cairo.h>
#include <stddef.h>
#include <stdint.h>

// Constants
#define DEFAULT_DPI (96.0)
#define INCH_PER_MM (25.4)
#define POINTS_PER_INCH (72.0)

// Check properties
#define CHECK_WIDTH_MM (152.4)
#define CHECK_HEIGHT_MM (70.0)
#define CHECK_PADX_MM (1.0)
#define CHECK_PADY_MM (1.0)
#define CHECK_FONT_HEIGHT (10)
#define CHECK_FONT ("Courier")
#define CHECK_VIEW_FONT_HEIGHT (9)
#define CHECK_VIEW_FONT ("Sans")

// Date
#define CHECK_DATE_POSX_MM (85.0)
#define CHECK_DATE_POSY_MM (19.0)
#define CHECK_DATE_WIDTH_MM (38.0)

// Name
#define CHECK_NAME_POSX_MM (16.0)
#define CHECK_NAME_POSY_MM (29.5)
#define CHECK_NAME_WIDTH_MM (97.0)

// Amount
#define CHECK_AMOUNT_POSX_MM (121.0)
#define CHECK_AMOUNT_POSY_MM (29.0)
#define CHECK_AMOUNT_WIDTH_MM (25.0)

// Amount in words
#define CHECK_AMOUNT_IN_WORDS_POSX_MM (6.0)
#define CHECK_AMOUNT_IN_WORDS_POSY_MM (37.0)
#define CHECK_AMOUNT_IN_WORDS_WIDTH_MM (113.0)

// Memo
#define CHECK_MEMO_POSX_MM (10.0)
#define CHECK_MEMO_POSY_MM (56.0)
#define CHECK_MEMO_WIDTH_MM (59.0)

#define CHECK_PREVIEW_ONLY (0x3)
#define CHECK_TEMPLATE (0x1)
#define CHECK_WRITE (0x0)

#define STRING_LEN (256)

typedef struct check_field_propperties
{
  double x_pos; // X position in mm
  double y_pos; // Y position in mm
  double width; // Width in mm
} FieldProperties;

// All fields are in millimeters
typedef struct check_properties
{
  double width;  // Width in mm
  double height; // Height in mm
  double x_pad;  // Horizontal padding in fields
  double y_pad;  // Vertical padding
  FieldProperties date;
  FieldProperties name;
  FieldProperties amount;
  FieldProperties amount_in_words;
  FieldProperties memo;
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

void check_properties_init (CheckProperties *p);

void check_data_init (CheckData *p);

int num_to_words (char *dst,
                  size_t len,
                  uint32_t num);

void render_check (cairo_t *cr,
                   const DisplayProperties *dprop,
                   const CheckProperties *cprop,
                   const CheckData *cdata,
                   int flags);

#endif /* CHECKWRITER_CEHCK_PROPERTIES_H_ */
