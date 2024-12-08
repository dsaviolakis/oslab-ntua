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
    printf("Data from %s: %s", device, buffer);  // Print without an additional newline
    close(fd);
}

// Function to read all sensors for a device without extra newlines between them
void read_device_sensors(int device_number, int is_first_reading) {
    char device_temp[64], device_batt[64], device_light[64];
    snprintf(device_temp, sizeof(device_temp), "/dev/lunix%d-temp", device_number);
    snprintf(device_batt, sizeof(device_batt), "/dev/lunix%d-batt", device_number);
    snprintf(device_light, sizeof(device_light), "/dev/lunix%d-light", device_number);

    if (!is_first_reading) {
        // Print a newline before printing data of the next device (separate different devices)
        printf("\n");
    }

    // Read all three sensors in sequence without extra newlines between them
    read_device(device_temp);
    read_device(device_batt);
    read_device(device_light);
}

// Function to handle continuous reading
void continuous_read(const char *input, int interval) {
    char device_name[64], sensor_name[64];
    int result = parse_sensor(input, device_name, sensor_name);

    int first_reading = 1;  // To handle proper newline between devices

    if (result == 1) {
        // Specific sensor
        while (1) {
            read_device(device_name);
            printf("\n");  // Add a newline after reading a specific sensor
            sleep(interval);
        }
    } else if (result == 0) {
        // Entire device (all sensors)
        int device_number = atoi(input);
        while (1) {
            read_device_sensors(device_number, first_reading);
            first_reading = 0;  // Set first_reading to 0 after the first device is read
            sleep(interval);
        }
    } else {
        fprintf(stderr, "Invalid input. Specify <device>, <sensor>, <range>, or 'all'.\n");
        exit(1);
    }
}
