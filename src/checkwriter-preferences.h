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

#ifndef CHECKWRITER_PREFERENCES_WINDOW_H
#define CHECKWRITER_PREFERENCES_WINDOW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS
#define CHECKWRITER_TYPE_PREFERENCES (checkwriter_preferences_get_type ())
G_DECLARE_FINAL_TYPE (CheckwriterPreferences, checkwriter_preferences, CHECKWRITER, PREFERENCES, GtkWidget)
G_END_DECLS

#define CHECKWRITER_PREFERENCES_RESOURCE_FILE ("/at/shafq/checkwriter/checkwriter-preferences.ui")

GtkWindow *checkwriter_preferences_window_new (GtkApplication *app);

#endif /* CHECKWRITER_PREFERENCES_WINDOW_H */
