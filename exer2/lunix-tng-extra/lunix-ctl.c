#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define IOCTL_SET_IO_MODE      _IOW('l', 1, int)  // Set blocking/non-blocking mode
#define IOCTL_SET_DATA_MODE    _IOW('l', 2, int)  // Set cooked/raw mode
#define BUFFER_SIZE 1024       // Buffer size for reading data

// Function to validate and parse IO mode
int parse_io_mode(const char *mode) {
    if (strcmp(mode, "blocking") == 0) return 0;
    if (strcmp(mode, "non-blocking") == 0) return 1;
    return -1;
}

// Function to validate and parse data mode
int parse_data_mode(const char *mode) {
    if (strcmp(mode, "cooked") == 0) return 0;
    if (strcmp(mode, "raw") == 0) return 1;
    return -1;
}

// Function to parse device range
int parse_range(const char *range, int *start, int *end) {
    if (strcmp(range, "all") == 0) {
        *start = 0;
        *end = 15;
        return 1;
    }
    if (sscanf(range, "%d-%d", start, end) == 2 && *start >= 0 && *end < 16 && *start <= *end) {
        return 1;
    }
    if (sscanf(range, "%d", start) == 1 && *start >= 0 && *start < 16) {
        *end = *start;
        return 1;
    }
    return 0;
}

// Function to configure a device
void configure_device(const char *device, int io_mode, int data_mode) {
    int fd = open(device, O_RDWR);
    if (fd < 0) {
        perror(device);
        return;
    }

    if (ioctl(fd, IOCTL_SET_IO_MODE, &io_mode) < 0) {
        perror("IO mode failed");
        close(fd);
        return;
    }

    if (ioctl(fd, IOCTL_SET_DATA_MODE, &data_mode) < 0) {
        perror("Data mode failed");
        close(fd);
        return;
    }

    printf("Configured %s: IO mode=%d, Data mode=%d\n", device, io_mode, data_mode);
    close(fd);
}

// Function to read from a device
void read_device(const char *device) {
    char buffer[BUFFER_SIZE];
    int fd = open(device, O_RDONLY);
    if (fd < 0) {
        perror(device);
        return;
    }

    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("Read failed");
        close(fd);
        return;
    }

    buffer[bytes_read] = '\0';
    printf("Data from %s: %s\n", device, buffer);
    close(fd);
}

// Function to configure devices in a range
void configure_range(int start, int end, int io_mode, int data_mode) {
    for (int i = start; i <= end; i++) {
        char device_temp[64], device_batt[64], device_light[64];
        snprintf(device_temp, sizeof(device_temp), "/dev/lunix%d-temp", i);
        snprintf(device_batt, sizeof(device_batt), "/dev/lunix%d-batt", i);
        snprintf(device_light, sizeof(device_light), "/dev/lunix%d-light", i);

        configure_device(device_temp, io_mode, data_mode);
        configure_device(device_batt, io_mode, data_mode);
        configure_device(device_light, io_mode, data_mode);
    }
}

// Function to read devices in a range
void read_range(int start, int end) {
    for (int i = start; i <= end; i++) {
        char device_temp[64], device_batt[64], device_light[64];
        snprintf(device_temp, sizeof(device_temp), "/dev/lunix%d-temp", i);
        snprintf(device_batt, sizeof(device_batt), "/dev/lunix%d-batt", i);
        snprintf(device_light, sizeof(device_light), "/dev/lunix%d-light", i);

        read_device(device_temp);
        read_device(device_batt);
        read_device(device_light);
    }
}

// Main function
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  Configure: %s configure <device|range|all> <io_mode> <data_mode>\n", argv[0]);
        fprintf(stderr, "  Read:      %s read <device|range|all>\n", argv[0]);
        return 1;
    }

    const char *command = argv[1];
    if (strcmp(command, "configure") == 0) {
        if (argc != 5) {
            fprintf(stderr, "Usage: %s configure <device|range|all> <io_mode> <data_mode>\n", argv[0]);
            return 1;
        }

        int io_mode = parse_io_mode(argv[3]);
        int data_mode = parse_data_mode(argv[4]);
        if (io_mode == -1 || data_mode == -1) {
            fprintf(stderr, "Invalid modes. IO modes: blocking, non-blocking. Data modes: cooked, raw.\n");
            return 1;
        }

        int start, end;
        if (parse_range(argv[2], &start, &end)) {
            configure_range(start, end, io_mode, data_mode);
        } else {
            fprintf(stderr, "Invalid device or range. Use <start>-<end>, single device, or 'all'.\n");
            return 1;
        }
    } else if (strcmp(command, "read") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s read <device|range|all>\n", argv[0]);
            return 1;
        }

        int start, end;
        if (parse_range(argv[2], &start, &end)) {
            read_range(start, end);
        } else {
            fprintf(stderr, "Invalid device or range. Use <start>-<end>, single device, or 'all'.\n");
            return 1;
        }
    } else {
        fprintf(stderr, "Unknown command '%s'. Use 'configure' or 'read'.\n", command);
        return 1;
    }

    return 0;
}
