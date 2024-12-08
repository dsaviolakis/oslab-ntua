#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>

#define IOCTL_SET_IO_MODE      _IOW('l', 1, int)  // Set blocking/non-blocking mode
#define IOCTL_SET_DATA_MODE    _IOW('l', 2, int)  // Set cooked/raw mode
#define BUFFER_SIZE 1024       // Buffer size for reading data
#define INTERVAL 1             // Constant interval of 1 second

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

// Function to determine if the input specifies a single sensor
int parse_sensor(const char *input, char *device_name, char *sensor_name) {
    int device_number;
    if (sscanf(input, "%d-%s", &device_number, sensor_name) == 2) {
        snprintf(device_name, 64, "/dev/lunix%d-%s", device_number, sensor_name);
        return 1; // Specific sensor
    } else if (sscanf(input, "%d", &device_number) == 1) {
        return 0; // Entire device (all sensors)
    }
    return -1; // Invalid input
}

// Function to configure a specific device or sensor
void configure_device(const char *device, int io_mode, int data_mode) {
    int fd = open(device, O_RDWR);
    if (fd < 0) {
        perror(device);
        return;
    }

    if (ioctl(fd, IOCTL_SET_IO_MODE, &io_mode) < 0) {
        perror("Failed to set IO mode");
        close(fd);
        return;
    }

    if (ioctl(fd, IOCTL_SET_DATA_MODE, &data_mode) < 0) {
        perror("Failed to set data mode");
        close(fd);
        return;
    }

    printf("Configured %s with IO mode %d and Data mode %d\n", device, io_mode, data_mode);
    close(fd);
}

// Function to configure all sensors for a device
void configure_device_sensors(int device_number, int io_mode, int data_mode) {
    char device_temp[64], device_batt[64], device_light[64];
    snprintf(device_temp, sizeof(device_temp), "/dev/lunix%d-temp", device_number);
    snprintf(device_batt, sizeof(device_batt), "/dev/lunix%d-batt", device_number);
    snprintf(device_light, sizeof(device_light), "/dev/lunix%d-light", device_number);

    configure_device(device_temp, io_mode, data_mode);
    configure_device(device_batt, io_mode, data_mode);
    configure_device(device_light, io_mode, data_mode);
}

// Function to handle the "configure" mode
void handle_configure_mode(char *argv[]) {
    int io_mode = parse_io_mode(argv[3]);
    int data_mode = parse_data_mode(argv[4]);

    if (io_mode == -1 || data_mode == -1) {
        fprintf(stderr, "Invalid modes. IO modes: blocking, non-blocking. Data modes: cooked, raw.\n");
        exit(1);
    }

    char device_name[64], sensor_name[64];
    int result = parse_sensor(argv[2], device_name, sensor_name);
    if (result == 1) {
        // Specific sensor
        configure_device(device_name, io_mode, data_mode);
    } else if (result == 0) {
        // Entire device (all sensors)
        int device_number = atoi(argv[2]);
        configure_device_sensors(device_number, io_mode, data_mode);
    } else {
        fprintf(stderr, "Invalid input. Specify <device>, <sensor>, <range>, or 'all'.\n");
        exit(1);
    }
}

// Function to read from a device or specific sensor
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

// Function to read all sensors for a device
void read_device_sensors(int device_number) {
    char device_temp[64], device_batt[64], device_light[64];
    snprintf(device_temp, sizeof(device_temp), "/dev/lunix%d-temp", device_number);
    snprintf(device_batt, sizeof(device_batt), "/dev/lunix%d-batt", device_number);
    snprintf(device_light, sizeof(device_light), "/dev/lunix%d-light", device_number);

    read_device(device_temp);
    read_device(device_batt);
    read_device(device_light);
}

// Function to handle continuous reading for specific sensors or devices
void continuous_read(const char *input) {
    char device_name[64], sensor_name[64];
    int result = parse_sensor(input, device_name, sensor_name);

    if (result == 1) {
        // Specific sensor, set interval to 1 second and read continuously
        while (1) {
            read_device(device_name);
            sleep(INTERVAL);  // Always 1 second
        }
    } else if (result == 0) {
        // Entire device (all sensors), only read once
        int device_number = atoi(input);
        read_device_sensors(device_number);
    } else {
        fprintf(stderr, "Invalid input. Specify <device>, <sensor>, <range>, or 'all'.\n");
        exit(1);
    }
}

// Function to handle the "read" mode
void handle_read_mode(char *argv[]) {
    continuous_read(argv[2]);  // Interval is 1 second
}

void print_usage(char *argv[]) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  Configure: %s configure <device|sensor|range|all> <io_mode> <data_mode>\n", argv[0]);
    fprintf(stderr, "    device format: <device_number> or <device_number>-<sensor_name> (e.g., 0, 0-temp, 0-11)\n");
    fprintf(stderr, "    io_mode: blocking or non-blocking\n");
    fprintf(stderr, "    data_mode: cooked or raw\n");
    fprintf(stderr, "  Read:      %s read <device|sensor|range|all>\n");
    fprintf(stderr, "    device format: <device_number> or <device_number>-<sensor_name> (e.g., 0, 0-temp, 0-11)\n", argv[0]);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv);
        return 1;
    }

    if (strcmp(argv[1], "configure") == 0) {
        if (argc != 5) {
            fprintf(stderr, "Usage: %s configure <device|sensor|range|all> <io_mode> <data_mode>\n", argv[0]);
            return 1;
        }
        handle_configure_mode(argv);
    } else if (strcmp(argv[1], "read") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: %s read <device|sensor|range|all>\n", argv[0]);
            return 1;
        }
        handle_read_mode(argv);
    } else {
        print_usage(argv);
        return 1;
    }

    return 0;
}
