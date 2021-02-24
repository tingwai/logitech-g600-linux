#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

struct input_event events[64];
const char kDir[] = "/dev/input/by-id/";
const char kPrefix[] = "usb-Logitech_Gaming_Mouse_G600_";
const char kSuffix[] = "-if01-event-kbd";

// ADD KEY->COMMAND MAPPINGS HERE:
const char *downCommands[] = {
  //[scancode] = "command to run",
  [4] = "python3 ./g600.py 'G-shift + G9' &", // scroll left
  [5] = "python3 ./g600.py 'G-shift + G10' &", // scroll right
  [6] = "python3 ./g600.py 'G-shift + G11' &", // G8
  [7] = "python3 ./g600.py 'G-shift + G12' &", // G7
  [8] = "python3 ./g600.py 'G-shift + G13' &", // G9
  [9] = "python3 ./g600.py 'G-shift + G14' &", // G10
  [10] = "python3 ./g600.py 'G-shift + G15' &", // G11
  [11] = "python3 ./g600.py 'G-shift + G16' &", // G12
  [12] = "python3 ./g600.py 'G-shift + G17' &", // G13
  [13] = "python3 ./g600.py 'G-shift + G18' &", // G14
  [14] = "python3 ./g600.py 'G-shift + G19' &", // G15
  [15] = "python3 ./g600.py 'G-shift + G20' &", // G16
  [30] = "python3 ./g600.py 'G9' &", // G-shift + G15
  [31] = "python3 ./g600.py 'G10' &", // G-shift + G16
  [32] = "python3 ./g600.py 'G11' &", // G-shift + G17
  [33] = "python3 ./g600.py 'G12' &", // G-shift + G18
  [34] = "python3 ./g600.py 'G13' &", // G-shift + G19
  [35] = "python3 ./g600.py 'G14' &", // G-shift + G20
  [36] = "python3 ./g600.py 'G15' &",
  [37] = "python3 ./g600.py 'G16' &",
  [38] = "python3 ./g600.py 'G17' &",
  [39] = "python3 ./g600.py 'G18' &",
  [45] = "python3 ./g600.py 'G19' &",
  [46] = "python3 ./g600.py 'G20' &",
};
const char *upCommands[] = {
  //[scancode] = "command to run",
  [37] = "echo button up"
};

int starts_with(const char* haystack, const char* prefix) {
  size_t prefix_length = strlen(prefix), haystack_length = strlen(haystack);
  if (haystack_length < prefix_length) return 0;
  return strncmp(prefix, haystack, prefix_length) == 0;
}

int ends_with(const char* haystack, const char* suffix) {
  size_t suffix_length = strlen(suffix), haystack_length = strlen(haystack);
  if (haystack_length < suffix_length) return 0;
  size_t haystack_end = haystack + haystack_length - suffix_length;
  return strncmp(suffix, haystack_end, suffix_length) == 0;
}

// Returns non-0 on error.
int find_g600(char *path) {
  //*path = kDir;
  DIR *dir;
  struct dirent *ent;
  if (!(dir = opendir(kDir))) {
    return 1;
  }
  while ((ent = readdir(dir))) {
    if (starts_with(ent->d_name, kPrefix) && ends_with(ent->d_name, kSuffix)) {
      strcpy(path, kDir);
      strcat(path, ent->d_name);

      printf("full path is %s\n", path);

      //*path += ent->d_name;
      closedir(dir);
      return 0;
    }
  }
  closedir(dir);
  return 2;
}

int main() {
  printf("Starting G600 Linux controller.\n\n");
  printf("It's a good idea to configure G600 with Logitech Gaming Software before running this program:\n");
  printf(" - assign left, right, middle mouse button and vertical mouse wheel to their normal functions\n");
  printf(" - assign the G-Shift button to \"G-Shift\"\n");
  printf(" - assign all other keys (including horizontal mouse wheel) to arbitrary (unique) keyboard keys\n");
  printf("\n");    
  char path[1024];
  int find_error = find_g600(&path);
  if (find_error) {
    printf("Error: Couldn't find G600 input device.\n");
    switch(find_error) {
    case 1: 
      printf("Suggestion: Maybe the expected directory (%s) is wrong. Check whether this directory exists and fix it by editing \"g600.c\".\n", kDir);
      break;
    case 2:
      printf("Suggestion: Maybe the expected device prefix (%s) is wrong. Check whether a device with this prefix exists in %s and fix it by editing \"g600.cpp\".\n", kPrefix, kDir);
      break;
    }
    printf("Suggestion: Maybe a permission is missing. Try running this program with with sudo.\n");
    return 1;
  }
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    printf("Error: Couldn't open \"%s\" for reading.\n", path);
    printf("Reason: %s.\n", strerror(errno));
    printf("Suggestion: Maybe a permission is missing. Try running this program with with sudo.\n");
    return 1;
  }

  ioctl(fd, EVIOCGRAB, 1);
  printf("G600 controller started successfully.\n\n");
  while (1) {
    size_t n = read(fd, events, sizeof(events));
    if (n <= 0) return 2;
    if (n < sizeof(struct input_event) * 2) continue;
    if (events[0].type != 4) continue;
    if (events[0].code != 4) continue;
    if (events[1].type != 1) continue;
    int pressed = events[1].value;
    int scancode = events[0].value & ~0x70000;

    const char* actionStr = (pressed) ? "Pressed" : "Released";
    printf("%s scancode %d.\n",actionStr, scancode);

    const char *downCommand = downCommands[scancode], *upCommand = upCommands[scancode];
    const char *cmdToRun = (pressed) ? downCommand : upCommand;
    if (!cmdToRun || !strlen(cmdToRun)) continue;

    printf("Executing: \"%s\"\n", cmdToRun);
    system(cmdToRun);
    printf("\n");
  }
  
  close(fd);
}
