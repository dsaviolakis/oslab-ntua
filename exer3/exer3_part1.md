## 2.3
### 2.3.1a
FS_DISK_1=./ext2-vdisks/fsdisk1-7c2b425a6.img  
FS_DISK_2=./ext2-vdisks/fsdisk2-a0173283d.img  
FS_DISK_3=./ext2-vdisks/fsdisk3-982902777.img  
exec $QEMU -enable-kvm -M pc -m $UTOPIA_MEMORY_MB \
    -smp 2 -drive file=$QCOW2_PRIVATE_FILE,if=virtio \
    -net nic -net user,hostfwd=tcp:$UTOPIA_SSH_INTERFACE:$UTOPIA_SSH_PORT-:22 \
    -vnc 127.0.0.1:0 \
    -nographic -monitor /dev/null  \
    -chardev socket,id=sensors0,host=lunix.cslab.ece.ntua.gr,port=49152,ipv4=on \
    -device isa-serial,chardev=sensors0,id=serial1,index=1 \
    -fsdev local,id=fsdev0,path="$SHARED_FS_DIR",security_model=none \
    -device virtio-9p-pci,fsdev=fsdev0,mount_tag=shared \
    -drive file=$FS_DISK_1,format=raw,if=virtio \
    $*
### 2.3.1b
ls /dev/ #vdb
### 2.3.2
lsblk #50M
### 2.3.3
mount /dev/vdb /mnt  
lsblk  
ls /mnt  
cat /proc/mounts #ext2
### 2.3.4
tune2fs -l /dev/vdb #Tue Dec 12 15:23:16 2023
### 2.3.5
tune2fs -l /dev/vdb #Mon Jan  6 22:06:15 2025  
### 2.3.6
tune2fs -l /dev/vdb #/mnt  
### 2.3.7
tune2fs -l /dev/vdb #Mon Jan  6 22:06:15 2025 ? 
### 2.3.9
tune2fs -l /dev/vdb #1024
### 2.3.11
tune2fs -l /dev/vdb #128
### 2.3.12
tune2fs -l /dev/vdb #49552 12810
### 2.3.16
dumpe2fs /dev/vdb #8193 16385 24577 32769 40961 49153
### 2.3.19
dumpe2fs /dev/vdb #7
### 2.3.22
dumpe2fs /dev/vdb #8194 16386 24578 32770 40962 49154
### 2.3.26
tune2fs -l /dev/vdb #8192 1832
### 2.3.27
ls -i /mnt/dir2/helloworld #9162
### 2.3.28
#9161/1832=5 ?
### 2.3.29
dumpe2fs /dev/vdb #Inode table at 40965-41193 (+4) ?
### 2.3.30
?
### 2.3.31
?
### 2.3.32
?
### 2.3.33
?
## 2.4

## 2.5
