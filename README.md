# Operating Systems Laboratory @ NTUA

## Students (equal contribution):

- [Alexandra Chelidoni](https://github.com/alexomegas)

- [Dimitrios Saviolakis](https://github.com/dsaviolakis)

## Assignment 1: Riddle Challenge

### Technical Overview
Reverse engineering challenge involving a Linux/x86-64 binary that presents a series of tests requiring environment manipulation.

### Technical Requirements
- Binary analysis without debuggers or LD_PRELOAD
- Environment variable manipulation
- File system state monitoring and modification
- Process execution context analysis
- System call interception and monitoring
- Signal handling analysis
- Resource limitation testing

### Technical Constraints
- No binary modification permitted
- Isolated execution environment required
- System call tracing allowed
- Environment variable manipulation permitted
- File system state changes allowed

## Assignment 2: Lunix:TNG Wireless Sensor Network Driver

### Technical Overview
Linux character device driver for Crossbow MPR2400CA wireless sensor network with battery voltage, temperature, and light sensors.

### Hardware Specifications
- Crossbow MPR2400CA sensor nodes
- MDA100CB sensor boards (voltage, temperature, light)
- MIB520CB USB base station interface
- Serial over USB communication protocol

### Kernel Module Architecture
- Character device driver implementation
- Multiple device files per sensor node (/dev/Lunix0-batt, /dev/Lunix0-temp, /dev/Lunix0-light)
- Linux kernel version 6.11 compatibility
- QEMU/KVM virtual machine deployment

### Driver Features
- Blocking read() system call implementation
- Multi-process synchronization with proper locking
- Optional memory-mapped I/O with mmap() support
- Sensor data buffering and management
- Line discipline integration for serial data processing

### Development Environment
- Debian-based utopia VM
- Custom Linux kernel 6.11 compilation
- Kernel module compilation and loading
- Virtual serial port simulation

## Assignment 3: ext2-lite File System

### Technical Overview
Simplified ext2 file system implementation as loadable Linux kernel module with VFS integration.

### File System Architecture
- Virtual File System (VFS) layer integration
- Superblock management with ext2_sb_info structure
- Inode operations with ext2_inode_info
- Block group descriptor management
- Directory entry handling with ext2_dirent

### Kernel Module Components
- File system registration with register_filesystem()
- Mount operation implementation with mount_bdev()
- Superblock operations structure (ext2_sops)
- Inode operations for different file types
- File operations for regular files and directories

### Core File System Operations
- Inode allocation and deallocation (ialloc.c)
- Block allocation and management (balloc.c)
- Directory operations: create, lookup, link, unlink, symlink
- File data read/write operations
- Page cache integration with address_space_operations

### Implementation Details
- ext2_super_block disk structure management
- ext2_inode disk structure handling
- Block and inode bitmap management
- Group descriptor table processing
- Directory entry creation and deletion
- Symbolic link handling (fast and slow symlinks)

### File System Features
- Regular file operations
- Directory creation and management
- Hard link and symbolic link support
- Block device file creation (mknod)
- File attribute management (getattr, setattr)
- File system statistics (statfs)

### Testing Environment
- File system image creation with mkfs.ext2
- Loop device mounting and unmounting
- Kernel module insertion and removal
- File system operations testing
- Cross-version compatibility verification

## Development Specifications

### Kernel Requirements
- Linux kernel 6.11 for Lunix:TNG driver
- Kernel headers for module compilation
- VFS API compatibility
- Character device driver framework

### Build System
- Kernel module Makefile configuration
- Cross-compilation for target kernel version
- Module dependency resolution
- Debug symbol inclusion

### Testing Methodology
- Virtual machine isolation for kernel development
- System call tracing and monitoring
- Kernel log analysis (dmesg)
- File system integrity verification
- Multi-process access pattern testing

### Technical Constraints
- Kernel space memory management requirements
- VFS interface compliance
- Blocking and non-blocking I/O support
- Proper reference counting implementatio
