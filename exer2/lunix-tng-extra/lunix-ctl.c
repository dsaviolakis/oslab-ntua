#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define IOCTL_SET_IO_MODE      _IOW('l', 1, int)  // Set blocking/non-blocking mode
#define IOCTL_SET_DATA_MODE    _IOW('l', 2, int)  // Set cooked/raw mode
#define BUFFER_SIZE 1024

// Function to handle the configuration of a device
void configure_device(const char *device, int io_mode, int data_mode) {
	int fd = open(device, O_RDWR);
	if (fd < 0) {
		perror("Failed to open device");
		return;
	}

	// Set IO mode
	if (ioctl(fd, IOCTL_SET_IO_MODE, &io_mode) < 0) {
		perror("Failed to set IO mode");
		close(fd);
		return;
	}
	printf("IO mode set to %d for device %s\n", io_mode, device);

	// Set data mode
	if (ioctl(fd, IOCTL_SET_DATA_MODE, &data_mode) < 0) {
		perror("Failed to set data mode");
		close(fd);
		return;
	}
	printf("Data mode set to %d for device %s\n", data_mode, device);

	// Close the device file
	close(fd);
}

// Function to handle the range of devices
void configure_range(int start, int end, int io_mode, int data_mode) {
	for (int i = start; i <= end; i++) {
		char device_temp[64], device_batt[64], device_light[64];

		// Create device names for each sensor
		snprintf(device_temp, sizeof(device_temp), "/dev/lunix%d-temp", i);
		snprintf(device_batt, sizeof(device_batt), "/dev/lunix%d-batt", i);
		snprintf(device_light, sizeof(device_light), "/dev/lunix%d-light", i);

		// Configure each sensor for this device
		configure_device(device_temp, io_mode, data_mode);
		configure_device(device_batt, io_mode, data_mode);
		configure_device(device_light, io_mode, data_mode);
	}
}

// Function to read data from a device
void read_device(const char *device) {
	char buffer[BUFFER_SIZE];
	int fd = open(device, O_RDONLY);
	if (fd < 0) {
		perror("Failed to open device");
		return;
	}

	ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
	if (bytes_read < 0) {
		perror("Failed to read from device");
		close(fd);
		return;
	}

	// Null-terminate the read data and print it
	buffer[bytes_read] = '\0';
	printf("Data from %s: %s\n", device, buffer);

	// Close the device file
	close(fd);
}

// Function to read data from a range of devices
void read_range(int start, int end) {
	for (int i = start; i <= end; i++) {
		char device_temp[64], device_batt[64], device_light[64];

		// Create device names for each sensor
		snprintf(device_temp, sizeof(device_temp), "/dev/lunix%d-temp", i);
		snprintf(device_batt, sizeof(device_batt), "/dev/lunix%d-batt", i);
		snprintf(device_light, sizeof(device_light), "/dev/lunix%d-light", i);

		// Read data from each sensor for this device
		read_device(device_temp);
		read_device(device_batt);
		read_device(device_light);
	}
}

int main(int argc, char *argv[]) {
	if (argc < 1) {
		fprintf(stderr, "Must provide arguments. Example usage:\n");
		fprintf(stderr, "Configure driver: %s configure <device_range|all> <io_mode> <data_mode>\n", argv[0]);
		fprintf(stderr, "Read data: %s read <device_range|all>\n", argv[0]);
		return 1;
	}

	if (strcmp(argv[1], "configure") == 0) {
		if (argc < 5) {
			fprintf(stderr, "Usage: %s configure <device_range|all> <io_mode> <data_mode>\n", argv[0]);
			return 1;
		}

		int io_mode = atoi(argv[3]);   // 0 for blocking, 1 for non-blocking
		int data_mode = atoi(argv[4]); // 0 for cooked, 1 for raw

		if (io_mode < 0 || io_mode > 1 || data_mode < 0 || data_mode > 1) {
			fprintf(stderr, "Invalid arguments: io_mode and data_mode must be 0 or 1\n");
			return 1;
		}
	
		if (strcmp(argv[2], "all") == 0) {
			// Configure all devices (lunix1-16)
			configure_range(1, 16, io_mode, data_mode);
		} else {
			// Handle range of devices (e.g., 1-5)
			char *range = argv[2];
			int start, end;

			if (sscanf(range, "%d-%d", &start, &end) == 2) {
				// Configure the range of devices
				if (start > 0 && end <= 16 && start <= end) {
					configure_range(start, end, io_mode, data_mode);
				} else {
					fprintf(stderr, "Invalid range specified\n");
					return 1;
				}
			} else {
			    // Invalid range format
			    fprintf(stderr, "Invalid range format. Use <start>-<end> or 'all'.\n");
			    return 1;
			}
		}
	} else if (strcmp(argv[1], "read")) {
		if (argc < 3) {
			fprintf(stderr, "Usage: %s read <device_range|all>\n", argv[0]);
			return 1;
		}

		if (strcmp(argv[2], "all") == 0) {
			// Read from all devices (lunix1-16)
			read_range(1, 16);
		} else {
			// Handle range of devices (e.g., 1-5)
			char *range = argv[2];
			int start, end;

			if (sscanf(range, "%d-%d", &start, &end) == 2) {
				// Read from the range of devices
				if (start > 0 && end <= 16 && start <= end) {
					read_range(start, end);
				} else {
		    			fprintf(stderr, "Invalid range specified\n");
		    			return 1;
				}
			} else {
				// Invalid range format
				fprintf(stderr, "Invalid range format. Use <start>-<end> or 'all'.\n");
				return 1;
			}
		}
	} else {
		fprintf(stderr, "Unknown command. Use 'configure' or 'read'.\n");
		return 1;
	}	
	return 0;
}


