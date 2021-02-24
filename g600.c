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
  // delegate command to python script with custom key bindings depending on program that's focused
  [4] = "python3 ./g600.py 'G_shift_G9' &",
  [5] = "python3 ./g600.py 'G_shift_G10' &",
  [6] = "python3 ./g600.py 'G_shift_G11' &",
  [7] = "python3 ./g600.py 'G_shift_G12' &",
  [8] = "python3 ./g600.py 'G_shift_G13' &",
  [9] = "python3 ./g600.py 'G_shift_G14' &",
  [10] = "python3 ./g600.py 'G_shift_G15' &",
  [11] = "python3 ./g600.py 'G_shift_G16' &",
  [12] = "python3 ./g600.py 'G_shift_G17' &",
  [13] = "python3 ./g600.py 'G_shift_G18' &",
  [14] = "python3 ./g600.py 'G_shift_G19' &",
  [15] = "python3 ./g600.py 'G_shift_G20' &",
  [30] = "python3 ./g600.py 'G9' &",
  [31] = "python3 ./g600.py 'G10' &",
  [32] = "python3 ./g600.py 'G11' &",
  [33] = "python3 ./g600.py 'G12' &",
  [34] = "python3 ./g600.py 'G13' &",
  [35] = "python3 ./g600.py 'G14' &",
  [36] = "python3 ./g600.py 'G15' &",
  [37] = "python3 ./g600.py 'G16' &",
  [38] = "python3 ./g600.py 'G17' &",
  [39] = "python3 ./g600.py 'G18' &",
  [45] = "python3 ./g600.py 'G19' &",
  [46] = "python3 ./g600.py 'G20' &",
};
const char *upCommands[] = {
  //[scancode] = "command to run",
  // I never press two buttons at the same time so safe to kill all python processes handling button inputs.
  // Also works as a hack to stop other buttons that failed to get killed
  [4] = "pkill -f 'python3 ./g600.py'",
  [5] = "pkill -f 'python3 ./g600.py'",
  [6] = "pkill -f 'python3 ./g600.py'",
  [7] = "pkill -f 'python3 ./g600.py'",
  [8] = "pkill -f 'python3 ./g600.py'",
  [9] = "pkill -f 'python3 ./g600.py'",
  [10] = "pkill -f 'python3 ./g600.py'",
  [11] = "pkill -f 'python3 ./g600.py'",
  [12] = "pkill -f 'python3 ./g600.py'",
  [13] = "pkill -f 'python3 ./g600.py'",
  [14] = "pkill -f 'python3 ./g600.py'",
  [15] = "pkill -f 'python3 ./g600.py'",
  [30] = "pkill -f 'python3 ./g600.py'",
  [31] = "pkill -f 'python3 ./g600.py'",
  [32] = "pkill -f 'python3 ./g600.py'",
  [33] = "pkill -f 'python3 ./g600.py'",
  [34] = "pkill -f 'python3 ./g600.py'",
  [35] = "pkill -f 'python3 ./g600.py'",
  [36] = "pkill -f 'python3 ./g600.py'",
  [37] = "pkill -f 'python3 ./g600.py'",
  [38] = "pkill -f 'python3 ./g600.py'",
  [39] = "pkill -f 'python3 ./g600.py'",
  [45] = "pkill -f 'python3 ./g600.py'",
  [46] = "pkill -f 'python3 ./g600.py'",
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
    // printf("\n");
  }
  
  close(fd);
}
