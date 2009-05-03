/*
 *  GL Widgets, event handling
 *  Copyright (C) 2008 Andreas Öman
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GLW_EVENT_H__
#define GLW_EVENT_H__

#include "event.h"


/** 
 *
 */
typedef struct glw_event_map {
  LIST_ENTRY(glw_event_map) gem_link;

  event_type_t gem_srcevent;

  void (*gem_fire)(glw_t *w, struct glw_event_map *gem, event_t *src);
  void (*gem_dtor)(struct glw_event_map *gem);

} glw_event_map_t;


/**
 *
 */
void glw_event_map_add(glw_t *w, glw_event_map_t *gem);

/**
 *
 */
void glw_event_map_destroy(glw_event_map_t *gem);

/**
 *
 */
int glw_event_map_intercept(glw_t *w, event_t *e);


/**
 *
 */
glw_event_map_t *glw_event_map_navOpen_create(const char *url, 
					      const char *type,
					      const char *parent);

/**
 *
 */
glw_event_map_t *glw_event_map_internal_create(const char *target,
					       event_type_t event);


/**
 *
 */
glw_event_map_t *glw_event_map_generic_create(event_type_t event);

#endif /* GLW_EVENT_H__ */
