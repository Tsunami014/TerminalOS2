#define _GNU_SOURCE
#include <stdio.h>
#include <sys/mount.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

extern int mainCode();

// Function to create /dev/input/event* device files
void create_input_devices() {
    mkdir("/dev/input", 0755);
    DIR *dir = opendir("/sys/class/input");
    if (!dir) {
        perror("Failed to open /sys/class/input");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "event", 5) == 0) {  // Look for "event*"
            char dev_path[256];
            snprintf(dev_path, sizeof(dev_path), "/dev/input/%s", entry->d_name);
            mknod(dev_path, S_IFCHR | 0666, makedev(13, atoi(entry->d_name + 5)));
        }
    }
    closedir(dir);
}

int main(void) {
    // Create necessary directories
    mkdir("/proc", 0755);
    mkdir("/sys", 0755);
    mkdir("/dev", 0755);
    mkdir("/run", 0755);
    mkdir("/tmp", 1777); // special permissions for /tmp
    
    // Mount necessary file systems
    if (mount("devtmpfs", "/dev", "devtmpfs", 0, NULL) < 0) {
        perror("Failed to mount /dev");
        while (1);
    }
    if (mount("proc", "/proc", "proc", 0, NULL) < 0) {
        perror("Failed to mount /proc");
        while (1);
    }
    if (mount("sysfs", "/sys", "sysfs", 0, NULL) < 0) {
        perror("Failed to mount /sys");
        while (1);
    }
    if (mount("tmpfs", "/run", "tmpfs", 0, "mode=0755") < 0) {
        perror("Failed to mount /proc");
        while (1);
    }
    if (mount("tmpfs", "/tmp", "tmpfs", 0, NULL) < 0) {
        perror("Failed to mount /sys");
        while (1);
    }
    
    // Create /dev/input/event* files automatically
    create_input_devices();
    
    // Open the console device file
    int console_fd = open("/dev/console", O_RDWR);
    if (console_fd >= 0) {
        ioctl(console_fd, TIOCSCTTY, 1);
        // Redirect stdin (0), stdout (1) and stderr (2) to /dev/console
        dup2(console_fd, 0);
        dup2(console_fd, 1);
        dup2(console_fd, 2);
        close(console_fd);
    } else {
        perror("Failed to open /dev/console");
        while (1);
    }

    int ret = mainCode();
    
    printf("\r\nProgram exited with code %d!", ret);
    // Fail-safe loop
    while (1);
    return 1;
}
