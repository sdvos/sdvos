/*
 *                   SDVOS System Generator
 *
 * Copyright (C) 2015 Ye Li (liye@sdvos.org)
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <parser.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
#include <sys/mman.h>

char * cur_file = NULL;

#ifdef PATH_MAX
static long pathmax = PATH_MAX;
#else
static long pathmax = 0;
#endif

static long posix_version = 0;
static long xsi_version = 0;

/* If PATH_MAX is indeterminate, no guarantee this is adequate */
#define PATH_MAX_GUESS 1024

/* path_alloc from APUE 3rd Edition */
char *
path_alloc (size_t *sizep)
{
  char * ptr = NULL;
  size_t size = 0;

  if (posix_version == 0)
    posix_version = sysconf (_SC_VERSION);
  if (xsi_version == 0)
    xsi_version = sysconf (_SC_XOPEN_VERSION);

  if (pathmax == 0) {  /* first time through */
    errno = 0;
    if ((pathmax = pathconf ("/", _PC_PATH_MAX)) < 0) {
      if (errno == 0)
        pathmax = PATH_MAX_GUESS;
      else
        fprintf (stderr, "pathconf error for _PC_PATH_MAX");
    } else {
      pathmax++;  /* add one since it’s relative to root */
    }
  }

  /*
   * Before POSIX.1-2001, we aren’t guaranteed that PATH_MAX includes
   * the terminating null byte. Same goes for XPG3.
   */
  if ((posix_version < 200112L) && (xsi_version < 4))
    size = pathmax + 1;
  else
    size = pathmax;
  if ((ptr = malloc (size)) == NULL)
    fprintf (stderr, "malloc error for pathname");
  if (sizep != NULL) *sizep = size;

  return (ptr);
}

void
sderror (char *s, int line, ...)
{
  va_list ap;
  va_start (ap, line);
  if (!cur_file) cur_file = "stdin";
  fprintf (stderr, "%s:%d: error: ", cur_file, line);
  vfprintf (stderr, s, ap);
  fprintf (stderr, "\n");
}

void
yyerror (char *s, ...)
{
  va_list ap;
  va_start (ap, s);
  if (!cur_file) cur_file = "stdin";
  fprintf (stderr, "%s:%d: error: ", cur_file, yylineno);
  vfprintf (stderr, s, ap);
  fprintf (stderr, "\n");
}

#define COPYINCR (1024*1024*1024)  /* 1 GB */
#define FILE_MODE                 \
  (S_IRUSR | S_IWUSR |            \
   S_IRGRP | S_IWGRP|             \
   S_IROTH | S_IWOTH)

/* File copy using mmap borrowed from APUE */
static int
copy_file (int fdin, const char * fileout)
{
  int fdout;
  void *src, *dst;
  size_t copysz;
  struct stat sbuf;
  off_t fsz = 0;
  extern int ftruncate (int fd, off_t length);

  if ((fdout = open (fileout,
      O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) < 0) {
    fprintf (stderr, "Can’t creat %s for writing", fileout);
    return -1;
  }

  if (fstat (fdin, &sbuf) < 0) {
    fprintf (stderr, "fstat error!\n");
    close (fdout);
    return -1;
  }

  /* need size of input file */
  if (ftruncate (fdout, sbuf.st_size) < 0) {  /* set output file size */
    fprintf (stderr, "ftruncate error!\n");
    close (fdout);
    return -1;
  }

  while (fsz < sbuf.st_size) {
    if ((sbuf.st_size - fsz) > COPYINCR)
      copysz = COPYINCR;
    else
      copysz = sbuf.st_size - fsz;

    if ((src = mmap (0, copysz, PROT_READ, MAP_SHARED,
                     fdin, fsz)) == MAP_FAILED) {
      fprintf (stderr, "mmap error for input!\n");
      close (fdout);
      return -1;
    }

    if ((dst = mmap (0, copysz, PROT_READ | PROT_WRITE,
                     MAP_SHARED, fdout, fsz)) == MAP_FAILED) {
      fprintf (stderr, "mmap error for output\n");
      close (fdout);
      return -1;
    }

    /* does the file copy */
    memcpy(dst, src, copysz);
    munmap(src, copysz);
    munmap(dst, copysz);
    fsz += copysz;
  }

  close (fdout);

  return 0;
}

static int
backup_and_copy (const char * path, FILE * src)
{
  char * back_path = NULL;
  struct stat st;
  int i = 0, j = 0;

  if (access (path, F_OK) == 0) {
    /* Path exists */
    if (stat (path, &st) < 0) {
      fprintf (stderr, "stat on %s failed!\n", path);
      return -1;
    }

    if (S_ISDIR(st.st_mode)) {
      fprintf (stderr, "%s exists but is a directory!\n", path);
      return -1;
    }

    if (bflag) {
      /* Backup existing file */
      back_path = malloc (strlen (path) + 2);
      j = strlen (path) + 1;
      for (i = strlen (path); i >= 0; i--) {
        if (path[i] == '/') {
          back_path[j] = '.';
          back_path[j - 1] = path[i];
          j--;
        } else {
          back_path[j] = path[i];
        }
        j--;
      }
      if (j == 0) back_path[0] = '.';

      if (access (back_path, F_OK) == 0) {
        /* Previous backup exists. Delete! */
        if (unlink (back_path) < 0) {
          fprintf (stderr, "Cannot remove %s!\n", back_path);
          return -1;
        }
      }

      if (rename (path, back_path) < 0) {
        fprintf (stderr, "Renaming %s to %s failed!\n", path, back_path);
        return -1;
      }
    }
  }

  if (copy_file (fileno (src), path) < 0) {
    fprintf (stderr, "%s copy failed!\n", path);
    return -1;
  }

  return 0;
}

char * apps_obj_list[MAX_APPS_OBJ];
static int apps_obj_list_index = 0;
static char * cur_apps_path = NULL;

static void
walk_source_tree (const char * path)
{
  struct stat st;
  DIR * dir = NULL;
  struct dirent * ent = NULL;

  if (access (path, F_OK) < 0) {
    fprintf (stderr, "Directory %s does not exist!\n", path);
    exit (1);
  } else {
    if (stat (path, &st) < 0) {
      fprintf (stderr, "stat on %s failed!\n", path);
      exit (1);
    }

    if (!S_ISDIR(st.st_mode)) {
      fprintf (stderr, "%s exists but is not a directory!\n", path);
      exit (1);
    }
  }

  if (!(dir = opendir (path))) {
    fprintf (stderr, "Cannot open apps directory!\n");
    exit (1);
  }

  if (chdir (path) < 0) {
    fprintf (stderr, "Cannot change directory to %s!\n", path);
    exit (1);
  }

  memcpy (cur_apps_path + strlen (cur_apps_path),
          path, strlen (path) + 1);
  cur_apps_path[strlen (cur_apps_path) + 1] = '\0';
  cur_apps_path[strlen (cur_apps_path)] = '/';

  while ((ent = readdir (dir))) {
    /* Recursively walk 'path' */
    if (stat (ent->d_name, &st) < 0) {
      fprintf (stderr, "stat on %s failed!\n", ent->d_name);
      exit (1);
    } else {
      if (S_ISDIR(st.st_mode)) {
        if ((strncmp (ent->d_name, "..", 3)) != 0 &&
            (strncmp (ent->d_name, ".", 2) != 0))
          walk_source_tree (ent->d_name);
      } else {
        if ((ent->d_name[strlen (ent->d_name) - 1] == 'c') &&
            (ent->d_name[strlen (ent->d_name) - 2] == '.')) {
          /* Check object file limit */
          if (apps_obj_list_index >= MAX_APPS_OBJ) {
            fprintf (stderr, "Too much objects in apps directory!\n");
            fprintf (stderr, "Current limit is: %d\n", MAX_APPS_OBJ);
            exit (1);
          }

          memcpy (cur_apps_path + strlen (cur_apps_path),
                  ent->d_name, strlen (ent->d_name) + 1);
          cur_apps_path[strlen (cur_apps_path) - 1] = 'o';
          apps_obj_list[apps_obj_list_index++] = strdup (cur_apps_path);
          cur_apps_path[strlen (cur_apps_path) -
                                strlen (ent->d_name)] = '\0';
        }
      }
    }
  }

  cur_apps_path[strlen (cur_apps_path) - strlen (path) - 1] = '\0';
  if (chdir ("..") < 0) {
    fprintf (stderr, "Cannot go back to parent directory!\n");
    exit (1);
  }
  closedir (dir);
}

bool
build_apps_config_obj_list ()
{
  char * cwd = getcwd (NULL, 0);
  size_t path_size = 0;

  if (!cwd) {
    fprintf (stderr, "Cannot get current working directory!\n");
    exit (1);
  }

  if (chdir (sdvos_root) < 0) {
    fprintf (stderr, "Cannot change directory to %s!\n", sdvos_root);
    exit (1);
  }

  memset (apps_obj_list, 0, sizeof (apps_obj_list));
  cur_apps_path = path_alloc (&path_size);
  memset (cur_apps_path, 0, path_size);

  /* Our current directory should already be SDVOS root */
  walk_source_tree ("apps");

  if (chdir (cwd) < 0) {
    fprintf (stderr, "Cannot change directory to %s!\n", cwd);
    exit (1);
  }

  return TRUE;
}

#define DIR_MODE                  \
  (S_IRUSR | S_IWUSR | S_IXUSR |  \
   S_IRGRP | S_IWGRP| S_IXGRP |   \
   S_IROTH)

int
copy_config_files (FILE * tmp_cfgmk, FILE * tmp_cfgh,
                   FILE * tmp_cfgc, FILE * tmp_appsmk)
{
  struct stat st;
  char * cwd = getcwd (NULL, 0);

  if (!cwd) {
    fprintf (stderr, "Cannot get current working directory!\n");
    exit (1);
  }

  /*
   * sdvos_root should be pointing to a valid directory. If not,
   * we are in trouble since a race condition happened...
   */
  if (chdir (sdvos_root) < 0) {
    fprintf (stderr, "Cannot change directory to %s!\n", sdvos_root);
    exit (1);
  }

  /* Our current directory should already be SDVOS root */
  if (access ("config", F_OK) < 0) {
    if (mkdir ("config", DIR_MODE) < 0) {
      fprintf (stderr, "Cannot create config directory!\n");
      exit (1);
    }
  } else {
    /* config exists. Is it a directory? */
    if (stat ("config", &st) < 0) {
      fprintf (stderr, "stat on config failed!\n");
      exit (1);
    }

    if (!S_ISDIR(st.st_mode)) {
      fprintf (stderr, "config exists but is not a directory!\n");
      exit (1);
    }
  }

  /* Now we have a config directory. Check existance of config files. */
  if (backup_and_copy ("config.mk", tmp_cfgmk) < 0) {
    if (bflag)
      fprintf (stderr, "Failed to backup and create config.mk!\n");
    else
      fprintf (stderr, "Failed to create config.mk!\n");
    exit (1);
  }

  if (backup_and_copy ("config/config.h", tmp_cfgh) < 0) {
    if (bflag)
      fprintf (stderr, "Failed to backup and create config.h!\n");
    else
      fprintf (stderr, "Failed to create config.h!\n");
    exit (1);
  }

  if (backup_and_copy ("config/config.c", tmp_cfgc) < 0) {
    if (bflag)
      fprintf (stderr, "Failed to backup and create config.c!\n");
    else
      fprintf (stderr, "Failed to create config.c!\n");
    exit (1);
  }

  if (backup_and_copy ("apps/config.mk", tmp_appsmk) < 0) {
    if (bflag)
      fprintf (stderr, "Failed to backup and create apps/config.mk!\n");
    else
      fprintf (stderr, "Failed to create apps/config.mk!\n");
    exit (1);
  }

  /* Change back to working directory */
  if (chdir (cwd) < 0) {
    fprintf (stderr, "Cannot change directory to %s!\n", cwd);
    exit (1);
  }

  return 0;
}

/* vi: set et ai sw=2 sts=2: */
