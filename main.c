#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>
#include <inttypes.h>
#include <string.h>

/// Retries before giving up opening devices
#define NUM_RETRY    5
///
#define NAME_SIZE    127

typedef struct {
   struct pollfd fds;
   struct input_event event;
   char name[NAME_SIZE];
} event_dev_t;

int getDevName(char *dev_path, char* name, int size) {
   int fd = -1;
   if ((fd = open(dev_path, O_RDONLY)) < 0) {
      perror("evdev open");
      return 0;
   }
   if (ioctl(fd, EVIOCGNAME(size), name) < 0) {
      perror("evdev ioctl");
      return 0;
   }
   close(fd);
   return 1;
}

int getKbBrightness(void) {
   FILE *fp;
   char path[256];
   int brightness = 0;

   /* Open the command for reading. */
   fp = popen("/bin/brightnessctl -d tpacpi::kbd_backlight get 2>&1", "r");
   if (fp == NULL) {
      printf("Failed to run command\n");
      return -1;
   }
   /* Read the output a line at a time - output it. */
   while (fgets(path, sizeof(path), fp) != NULL) {
      printf("%s", path);
      brightness = atoi(path);
   }
   /* close */
   pclose(fp);
   return brightness;
}

int setKbBrightness(int brightness) {
   FILE *fp;
   char path[256];
   char command[256];
   sprintf(command, "/bin/brightnessctl -d tpacpi::kbd_backlight set %d 2>&1", brightness);
   /* Open the command for writing. */
   fp = popen(command, "w");
   if (fp == NULL) {
      printf("Failed to run command\n");
      return -1;
   }
   /* Read the output a line at a time - output it. */
   while (fgets(path, sizeof(path), fp) != NULL) {
      printf("%s", path);
   }
   /* close */
   pclose(fp);
   return 0;
}

// Argument order is path, idle time, <input devices seperated by white spaces>
int main(int argc, char *argv[]) {
   if (argc < 3) {
      fprintf(stderr, "Provide at least two arguments: <timeout> <device_path>\n");
      exit(1);
   }
   // First two arguments are <path of working dir> and <timeout>
   const int devList_offset = 2;
   const int num_dev = argc - devList_offset;

   event_dev_t evt_dev[num_dev];

   // Open event devices for reading
   for (int i = 0; i < num_dev; i++) { 
      uint8_t retry_ctr = 0;
      uint8_t fd_opened = 0;
      if (!getDevName(argv[i+devList_offset], evt_dev[i].name, NAME_SIZE)) {
         fprintf(stderr, "Failed getting device name\n");
         strcpy(evt_dev[i].name, "unknown");
      }
      while (!fd_opened) {
         evt_dev[i].fds.fd = open(argv[i+devList_offset], O_RDONLY);
         evt_dev[i].fds.events = POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI;
         if (evt_dev[i].fds.fd < 0) {
            fprintf(stderr, "Failed opening event device %s.\n", evt_dev[i].name);
            fd_opened = 0;
            sleep(1);
         } else {
            fprintf(stdout, "Successfully opened device %s\n", evt_dev[i].name);
            fd_opened = 1;
         }
         retry_ctr++;
         if (retry_ctr >= NUM_RETRY) {
            fprintf(stderr, "Giving up :(\n");
            exit(1);
         }
      }
   }

   // Timestamp of last activity
   int t0 = (int) time(NULL);

   // Timeout after which keyboard backlight is turned off
   int timeout = atoi(argv[1]);
   int asleep = 0;
   int prev_brightness = 0;

   while (1) {
      for (int i = 0; i < num_dev; i++) {
         // Check if there is some data ready to be read
         if (poll(&evt_dev[i].fds, 1, 1)) {
            if (evt_dev[i].fds.events != 0) {
               if (read(evt_dev[i].fds.fd, &evt_dev[i].event, sizeof(struct input_event)) < 0) {
                  fprintf(stderr, "Failed to read from %s\n", evt_dev[i].name);
                  exit(1);
               }
               if (evt_dev[i].event.type != 0) {
                  /** printf("Key event!\n"); */
                  t0 = (int) time(NULL);
                  /** printf("Timestamp: %d\n", t0); */
                  if (asleep) {
                     asleep = 0;
                     printf("Time to wake up :)\n");
                     setKbBrightness(prev_brightness);
                  }
               }
            }
         }
      }
      // No Event for 'timeout' seconds occoured
      if ((int) time(NULL) - t0 > timeout && !asleep) {
         printf("Time to sleep!\n");
         asleep = 1;
         prev_brightness = getKbBrightness();
         setKbBrightness(0);
      }
   }
}
