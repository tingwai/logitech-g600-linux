#include <linux/input.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>

input_event events[64];
const char kDir[] = "/dev/input/by-id/";
const char kPrefix[] = "usb-Logitech_Gaming_Mouse_G600_";
const char kSuffix[] = "-if01-event-kbd";

struct Command {
  const int scancode;
  const char* command;
  const char* command_up;  //Executed command when user stops pressing the button
};

// ADD KEY->COMMAND MAPPINGS HERE:
const Command kCommands[] = {
  { 4, "python3.6 ./g600.py 'G-shift + G9' &" },
  { 5, "python3.6 ./g600.py 'G-shift + G10' &" },
  { 6, "python3.6 ./g600.py 'G-shift + G11' &" },
  { 7, "python3.6 ./g600.py 'G-shift + G12' &" },
  { 8, "python3.6 ./g600.py 'G-shift + G13' &" },
  { 9, "python3.6 ./g600.py 'G-shift + G14' &" },
  { 10, "python3.6 ./g600.py 'G-shift + G15' &" },
  { 11, "python3.6 ./g600.py 'G-shift + G16' &" },
  { 12, "python3.6 ./g600.py 'G-shift + G17' &" },
  { 13, "python3.6 ./g600.py 'G-shift + G18' &" },
  { 14, "python3.6 ./g600.py 'G-shift + G19' &" },
  { 15, "python3.6 ./g600.py 'G-shift + G20' &" },
  { 30, "python3.6 ./g600.py 'G9' &" },
  { 31, "python3.6 ./g600.py 'G10' &" },
  { 32, "python3.6 ./g600.py 'G11' &" },
  { 33, "python3.6 ./g600.py 'G12' &" },
  { 34, "python3.6 ./g600.py 'G13' &" },
  { 35, "python3.6 ./g600.py 'G14' &" },
  { 36, "python3.6 ./g600.py 'G15' &" },
  { 37, "python3.6 ./g600.py 'G16' &" },
  { 38, "python3.6 ./g600.py 'G17' &" },
  { 39, "python3.6 ./g600.py 'G18' &" },
  { 45, "python3.6 ./g600.py 'G19' &" },
  { 46, "python3.6 ./g600.py 'G20' &" },
};

bool starts_with(const char* haystack, const char* prefix) {
  auto prefix_length = strlen(prefix), haystack_length = strlen(haystack);
  if (haystack_length < prefix_length) return false;
  return strncmp(prefix, haystack, prefix_length) == 0;
}

bool ends_with(const char* haystack, const char* suffix) {
  auto suffix_length = strlen(suffix), haystack_length = strlen(haystack);
  if (haystack_length < suffix_length) return false;
  auto haystack_end = haystack + haystack_length - suffix_length;
  return strncmp(suffix, haystack_end, suffix_length) == 0;
}

// Returns non-0 on error.
int find_g600(std::string* path) {
  *path = kDir;
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(kDir)) == nullptr) {
    return 1;
  }
  while ((ent = readdir(dir))) {
    if (starts_with(ent->d_name, kPrefix) && ends_with(ent->d_name, kSuffix)) {
      *path += ent->d_name;
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
  std::string path;
  int find_error = find_g600(&path);
  if (find_error) {
    printf("Error: Couldn't find G600 input device.\n");
    switch(find_error) {
    case 1: 
      printf("Suggestion: Maybe the expected directory (%s) is wrong. Check whether this directory exists and fix it by editing \"g600.cpp\".\n", kDir);
      break;
    case 2:
      printf("Suggestion: Maybe the expected device prefix (%s) is wrong. Check whether a device with this prefix exists in %s and fix it by editing \"g600.cpp\".\n", kPrefix, kDir);
      break;
    }
    printf("Suggestion: Maybe a permission is missing. Try running this program with with sudo.\n");
    return 1;
  }
  int fd = open(path.c_str(), O_RDONLY);
  if (fd < 0) {
    printf("Error: Couldn't open \"%s\" for reading.\n", path.c_str());
    printf("Reason: %s.\n", strerror(errno));
    printf("Suggestion: Maybe a permission is missing. Try running this program with with sudo.\n");
    return 1;
  }

  ioctl(fd, EVIOCGRAB, 1);
  printf("G600 controller started successfully.\n\n");
  while (1) {
    size_t n = read(fd, events, sizeof(events));
    if (n <= 0) return 2;
    if (n < sizeof(input_event) * 2) continue;
    if (events[0].type != 4) continue;
    if (events[0].code != 4) continue;
    if (events[1].type != 1) continue;
    bool pressed = events[1].value;
    int scancode = events[0].value & ~0x70000;

    const Command* cmd = nullptr;
    for (size_t i = 0; i < sizeof(kCommands) / sizeof(Command); ++i) {
      if (kCommands[i].scancode == scancode) {
        cmd = &kCommands[i];
        break;
      }
    }

    if (cmd == nullptr) {
      if(pressed){
        printf("Warning: Pressed a key (%d) without a mapping.\n", scancode);
        printf("Suggestion: Add a mapping by editing \"g600.cpp\".\n");
        printf("\n");
      }
      continue;
    }
    const char* cmdToRun = (pressed) ? cmd->command : cmd->command_up;
    if (!cmdToRun) continue;
    const char* actionStr = (pressed) ? "Pressed" : "Released";
    printf("%s Scancode: %d. Command: \"%s\"\n",actionStr, scancode, cmdToRun);
    if (strlen(cmdToRun) == 0) continue;
    system(cmdToRun);
    printf("Subprocess finished.\n");
  }
  
  close(fd);
}
