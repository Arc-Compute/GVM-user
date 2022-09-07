/*
 * Copyright (C) 2022 2666680 Ontario Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#ifndef UTILS_COLORS_H
#define UTILS_COLORS_H

#ifdef __cplusplus
extern "C" {
#endif

//! Prefix for colors.
#define COLOR_PREFIX "\033["

//! Red color.
#define RED_COLOR COLOR_PREFIX "31m"

//! Green color.
#define GREEN_COLOR COLOR_PREFIX "32m"

//! Yellow color.
#define YELLOW_COLOR COLOR_PREFIX "33m"

//! Blue color.
#define BLUE_COLOR COLOR_PREFIX "34m"

//! Magenta color.
#define MAGENTA_COLOR COLOR_PREFIX "35m"

//! Cyan color.
#define CYAN_COLOR COLOR_PREFIX "36m"

//! White color.
#define WHITE_COLOR COLOR_PREFIX "37m"

//! Reset color.
#define RESET_COLOR COLOR_PREFIX "0m"

#ifdef __cplusplus
};
#endif

#endif
