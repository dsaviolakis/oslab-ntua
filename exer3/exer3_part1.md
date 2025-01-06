## 2.3
### 2.3.1
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

## 2.4

## 2.5
