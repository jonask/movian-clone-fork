/*
 *  Showtime GTK frontend
 *  Copyright (C) 2009 Andreas Öman
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

#include <assert.h>
#include <string.h>
#include "gu.h"
#include "showtime.h"

/**
 *
 */
static void
gu_nav_page_clean(gu_nav_page_t *gnp)
{
  if(gnp->gnp_destroy == NULL)
    return;
  gnp->gnp_destroy(gnp->gnp_opaque);
  gnp->gnp_destroy = NULL;
}


/**
 *
 */
static void
gu_nav_page_set_type(void *opaque, const char *type)
{
  gu_nav_page_t *gnp = opaque;

  gu_nav_page_clean(gnp);

  if(type == NULL)
    return;

  if(!strcmp(type, "directory")) {
    gu_directory_create(gnp);
  } else if(!strcmp(type, "playqueue")) {
    gu_directory_create(gnp);
  } else {
    TRACE(TRACE_ERROR, "GU", "Can not display page type: %s", type);
  }
}


/**
 *
 */
static void
gu_nav_page_set_url(void *opaque, const char *url)
{
  gu_nav_page_t *gnp = opaque;
  free(gnp->gnp_url);
  gnp->gnp_url = url ? strdup(url) : NULL;
}


/**
 *
 */
static gu_nav_page_t *
gu_nav_page_create(gtk_ui_t *gu, prop_t *p)
{
  gu_nav_page_t *gnp = calloc(1, sizeof(gu_nav_page_t));

  gnp->gnp_gu = gu;
  gnp->gnp_prop = p;
  prop_ref_inc(p);

  gnp->gnp_rootbox = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(gnp->gnp_rootbox), 0);
  gtk_container_add(GTK_CONTAINER(gu->gu_page_container), gnp->gnp_rootbox);
  gtk_widget_show(gnp->gnp_rootbox);

  LIST_INSERT_HEAD(&gu->gu_pages, gnp, gnp_link);

  gnp->gnp_sub_type = 
    prop_subscribe(0,
		   PROP_TAG_NAME("self", "type"),
		   PROP_TAG_CALLBACK_STRING, gu_nav_page_set_type, gnp,
		   PROP_TAG_COURIER, gu->gu_pc, 
		   PROP_TAG_NAMED_ROOT, p, "self",
		   NULL);

  gnp->gnp_sub_url =
    prop_subscribe(0,
		   PROP_TAG_NAME("self", "url"),
		   PROP_TAG_CALLBACK_STRING, gu_nav_page_set_url, gnp,
		   PROP_TAG_COURIER, gu->gu_pc, 
		   PROP_TAG_NAMED_ROOT, p, "self",
		   NULL);

  return gnp;
}


/**
 *
 */
static void
gu_nav_page_destroy(gu_nav_page_t *gnp)
{
  gu_nav_page_clean(gnp);

  prop_unsubscribe(gnp->gnp_sub_type);
  prop_unsubscribe(gnp->gnp_sub_url);

  gtk_widget_destroy(gnp->gnp_rootbox);

  LIST_REMOVE(gnp, gnp_link);
  
  prop_ref_dec(gnp->gnp_prop);
  free(gnp->gnp_url);
  free(gnp);
}


/**
 *
 */
static gu_nav_page_t *
gu_nav_page_find(gtk_ui_t *gu, prop_t *p)
{
  gu_nav_page_t *gnp;

  LIST_FOREACH(gnp, &gu->gu_pages, gnp_link)
    if(p == gnp->gnp_prop)
      break;
  return gnp;
}


/**
 *
 */
static void
gu_nav_page_display(gtk_ui_t *gu, gu_nav_page_t *gnp)
{
  if(gu->gu_page_current == gnp)
    return;

  if(gu->gu_page_current != NULL)
    gtk_widget_hide(gu->gu_page_current->gnp_rootbox);

  gu->gu_page_current = gnp;
  gtk_widget_show(gnp->gnp_rootbox);
}



/**
 *
 */
void
gu_nav_pages(void *opaque, prop_event_t event, ...)
{
  gtk_ui_t *gu = opaque;
  prop_t *p;
  int flags;
  gu_nav_page_t *gnp;

  va_list ap;
  va_start(ap, event);
  
  switch(event) {
  case PROP_ADD_CHILD:
    p = va_arg(ap, prop_t *);
    flags = va_arg(ap, int);
    gnp = gu_nav_page_create(gu, p);

    if(flags & PROP_ADD_SELECTED)
      gu_nav_page_display(gu, gnp);
    break;

  case PROP_SELECT_CHILD:
    p = va_arg(ap, prop_t *);
    gnp = gu_nav_page_find(gu, p);
    assert(gnp != NULL);
    gu_nav_page_display(gu, gnp);
    break;

  case PROP_DEL_CHILD:
    p = va_arg(ap, prop_t *);
    gnp = gu_nav_page_find(gu, p);
    assert(gnp != NULL);
    gu_nav_page_destroy(gnp);
    break;

  case PROP_SET_DIR:
  case PROP_SET_VOID:
    break;

  default:
    fprintf(stderr, 
	    "gu_nav_pages(): Can not handle event %d, aborting()\n", event);
    abort();
  }
}
