/*
 *  Various FS helpers
 *  Copyright (C) 2010 Andreas Öman
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <alloca.h>

#include <errno.h>

#include "fs.h"

/**
 *
 */
int
makedirs(const char *path)
{
  struct stat st;
  char *p;
  int l, r;

  if(stat(path, &st) == 0 && S_ISDIR(st.st_mode)) 
    return 0; /* Dir already there */

  if(mkdir(path, 0777) == 0)
    return 0; /* Dir created ok */

  if(errno == ENOENT) {

    /* Parent does not exist, try to create it */
    /* Allocate new path buffer and strip off last directory component */

    l = strlen(path);
    p = alloca(l + 1);
    memcpy(p, path, l);
    p[l--] = 0;
  
    for(; l >= 0; l--)
      if(p[l] == '/')
	break;
    if(l == 0) {
      return ENOENT;
    }
    p[l] = 0;

    if((r = makedirs(p)) != 0)
      return r;
  
    /* Try again */
    if(mkdir(path, 0777) == 0)
      return 0; /* Dir created ok */
  }
  r = errno;
  return r;
}
