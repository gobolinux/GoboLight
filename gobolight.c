
#include <dirent.h>
#include <err.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#ifndef CLAMP
#define CLAMP(x,a,b) ((x)<(a)?(a):((x)>(b)?(b):(x)))
#endif

enum modes {
   INC,
   DEC,
   GET,
   SET
};

int readFile(const char* filename) {
   FILE* fd = fopen(filename, "r");
   if (!fd) {
      err(1, filename);
   }

   char buffer[256];
   char* rc = fgets(buffer, 255, fd);
   if (rc != buffer) {
      err(1, filename);
   }

   buffer[255] = '\0';
   int n = strtol(buffer, NULL, 10);
   if (errno != 0) {
      err(1, filename);
   }

   fclose(fd);
   return n;
}

int setBrightness(uid_t user, int units) {
   seteuid(0);
   FILE* fd = fopen("brightness", "w");
   setuid(user);
   if (!fd) {
      err(1, "brightness");
   }

   fprintf(fd, "%d", units);
   int ret = errno;

   fclose(fd);
   return ret;
}

int main(int argc, char** argv) {

   uid_t user = getuid();
   seteuid(user);

   enum modes mode = GET;
   int value = 0;

   if (argc > 1) {
      int v = 0;
      if (argc == 2) {
         mode = SET;
         v = 1;
      } else if (argc == 3) {
         if (strcmp(argv[1], "-inc") == 0) {
            mode = INC;
         } else if (strcmp(argv[1], "-dec") == 0) {
            mode = DEC;
         } else {
            errx(1, "invalid argument: %s", argv[1]);
         }
         v = 2;
      } else {
         errx(1, "invalid number of arguments");
      }
      char* endptr;
      value = strtol(argv[v], &endptr, 10);
      if (errno != 0 || *endptr != '\0') {
         errx(1, "invalid argument: %s", argv[v]);
      }
      if (value < 0 || value > 100) {
         errx(1, "value out of range (not between 0 and 100): %d", value);
      }
   }

   DIR* dirp = opendir("/sys/class/backlight");
   if (!dirp) {
      err(1, "/sys/class/backlight");
   }

   int rc = chdir("/sys/class/backlight");
   if (rc != 0) {
      err(1, "/sys/class/backlight");
   }

   struct dirent* ent;
   const char* name;
   get_name:
   ent = readdir(dirp);
   if (!ent) {
      err(1, "no backlight");
   }
   name = ent->d_name;
   if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
      goto get_name;
   }

   rc = chdir(ent->d_name);
   if (rc != 0) {
      err(1, "/sys/class/backlight/%s", ent->d_name);
   }
   
   int brightness = readFile("brightness");
   int maxBrightness = readFile("max_brightness");

   int units = (int)((double)value / 100.0 * maxBrightness);

   int ret = 0;
   switch (mode) {
      case SET:
         ret = setBrightness(user, units);
         break;
      case INC:
         ret = setBrightness(user, CLAMP(brightness + units, 0, maxBrightness));
         break;
      case DEC:
         ret = setBrightness(user, CLAMP(brightness - units, 0, maxBrightness));
         break;
   }
   if (mode != GET) {
      brightness = readFile("brightness");
   }
   
   printf("%d\n", (int)ceil((double)brightness / maxBrightness * 100.0));
   return ret;
}
