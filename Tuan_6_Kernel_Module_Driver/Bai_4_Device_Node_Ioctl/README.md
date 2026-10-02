# BÀI 4 — CHARACTER DEVICE DRIVER VỚI `ioctl()`

> **Tuần 6 — Linux Kernel Module & Driver**  
> Mở rộng Character Device Driver của Bài 3 bằng cơ chế `ioctl()` để User Space có thể gửi lệnh điều khiển xuống Kernel Driver.

---

## 1. Mục tiêu

Sau bài này cần nắm được:

- Cách thêm `ioctl()` vào Character Device Driver.
- Cách khai báo `IOCTL_CLEAR` và `IOCTL_GET_LEN`.
- Cách ánh xạ `.unlocked_ioctl` trong `struct file_operations`.
- Cách gửi command từ User Space xuống Kernel Space.
- Cách trả dữ liệu từ Kernel Space về User Space bằng `copy_to_user()`.
- Cách viết chương trình User Space để test `ioctl()`.
- Phân biệt:
  - Kernel Module: `mychardev_ioctl`
  - Device Node: `/dev/mychardev`

---

## 2. `ioctl()` là gì?

`ioctl()` là viết tắt của:

```text
Input / Output Control
```

Nó dùng để gửi **lệnh điều khiển đặc biệt** từ User Space xuống Driver.

```text
read()
  |
  v
Đọc dữ liệu từ Driver

write()
  |
  v
Ghi dữ liệu vào Driver

ioctl()
  |
  v
Gửi lệnh điều khiển cho Driver
```

Trong bài này:

```text
IOCTL_GET_LEN
    |
    v
Lấy số byte hiện có trong kernel_buffer

IOCTL_CLEAR
    |
    v
Xóa toàn bộ kernel_buffer
```

---

## 3. Quy trình tổng quát

```text
User Space
    |
    | open("/dev/mychardev")
    v
fd
    |
    | ioctl(fd, command, ...)
    v
Linux Kernel
    |
    v
file_operations
    |
    v
.unlocked_ioctl
    |
    v
my_ioctl()
    |
    +-----------------------+
    |                       |
    v                       v
IOCTL_GET_LEN          IOCTL_CLEAR
    |                       |
    v                       v
Lấy data_size         Xóa kernel_buffer
    |                       |
    v                       v
copy_to_user()        data_size = 0
```

---

## 4. Cấu trúc thư mục

```text
Bai_4_Device_Node_Ioctl/
├── mychardev_ioctl.c
├── test_ioctl.c
├── Makefile
├── mychardev_ioctl.ko
├── mychardev_ioctl.o
├── mychardev_ioctl.mod.c
├── mychardev_ioctl.mod.o
├── Module.symvers
├── modules.order
└── *.cmd
```

| File | Vai trò |
|---|---|
| `mychardev_ioctl.c` | Source code Kernel Driver |
| `test_ioctl.c` | Chương trình User Space test `ioctl()` |
| `Makefile` | Build module và chương trình test |
| `mychardev_ioctl.ko` | Kernel Module hoàn chỉnh |
| `*.o`, `*.cmd` | File build trung gian |
| `Module.symvers` | Symbol/version information |
| `modules.order` | Thứ tự module build |

---

## 5. Các lệnh `ioctl`

### 5.1. Magic Number

```c
#define MY_MAGIC 'M'
```

Dùng để nhận dạng nhóm command của driver.

### 5.2. Lệnh CLEAR

```c
#define IOCTL_CLEAR _IO(MY_MAGIC, 1)
```

Ý nghĩa:

```text
Magic Number : 'M'
Command No.  : 1
Data         : không truyền thêm dữ liệu
```

Dùng để:

```text
Xóa kernel_buffer
data_size = 0
```

### 5.3. Lệnh GET_LEN

```c
#define IOCTL_GET_LEN _IOR(MY_MAGIC, 2, int)
```

`R` trong `_IOR` có thể nhớ là:

```text
Read
```

Tức là User Space nhận dữ liệu từ Kernel.

---

## 6. Hàm `my_ioctl()`

```c
static long my_ioctl(struct file *file,
                     unsigned int cmd,
                     unsigned long arg)
{
    int len;

    switch (cmd) {

    case IOCTL_CLEAR:
        memset(kernel_buffer, 0, BUFFER_SIZE);
        data_size = 0;

        pr_info("mychardev: buffer cleared\n");
        break;

    case IOCTL_GET_LEN:
        len = data_size;

        if (copy_to_user((int __user *)arg,
                         &len,
                         sizeof(len)))
            return -EFAULT;

        pr_info("mychardev: buffer length = %d\n", len);
        break;

    default:
        return -EINVAL;
    }

    return 0;
}
```

### Ý nghĩa tham số

| Tham số | Ý nghĩa |
|---|---|
| `file` | Device/file đang được mở |
| `cmd` | Lệnh ioctl được gửi xuống |
| `arg` | Dữ liệu hoặc địa chỉ dữ liệu bổ sung |

Luồng:

```text
ioctl(fd, IOCTL_GET_LEN, &len)
              |
              v
Kernel
              |
              v
my_ioctl()
              |
              +--> cmd = IOCTL_GET_LEN
              |
              +--> arg = địa chỉ của len
```

---

## 7. `file_operations`

```c
static const struct file_operations fops = {
    .owner          = THIS_MODULE,
    .open           = my_open,
    .read           = my_read,
    .write          = my_write,
    .release        = my_release,
    .unlocked_ioctl = my_ioctl,
};
```

Ánh xạ:

```text
open()   -> .open            -> my_open()
read()   -> .read            -> my_read()
write()  -> .write           -> my_write()
ioctl()  -> .unlocked_ioctl  -> my_ioctl()
```

---

## 8. Chương trình User Space `test_ioctl.c`

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_FILE "/dev/mychardev"

#define MY_MAGIC      'M'
#define IOCTL_CLEAR   _IO(MY_MAGIC, 1)
#define IOCTL_GET_LEN _IOR(MY_MAGIC, 2, int)

int main(void)
{
    int fd;
    int len;

    fd = open(DEVICE_FILE, O_RDWR);

    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Device opened successfully\n");

    if (ioctl(fd, IOCTL_GET_LEN, &len) < 0) {
        perror("IOCTL_GET_LEN");
        close(fd);
        return 1;
    }

    printf("Buffer length = %d bytes\n", len);

    printf("Clearing buffer...\n");

    if (ioctl(fd, IOCTL_CLEAR) < 0) {
        perror("IOCTL_CLEAR");
        close(fd);
        return 1;
    }

    if (ioctl(fd, IOCTL_GET_LEN, &len) < 0) {
        perror("IOCTL_GET_LEN");
        close(fd);
        return 1;
    }

    printf("Buffer length after clear = %d bytes\n", len);

    close(fd);

    return 0;
}
```

---

## 9. Giải thích `ioctl(fd, IOCTL_CLEAR)`

```c
ioctl(fd, IOCTL_CLEAR);
```

Có thể hiểu là:

```text
Gửi lệnh IOCTL_CLEAR
        |
        v
tới device được đại diện bởi fd
        |
        v
/dev/mychardev
        |
        v
Linux Kernel
        |
        v
my_ioctl()
        |
        v
Xóa kernel_buffer
```

Trong đó `fd` được lấy từ:

```c
fd = open("/dev/mychardev", O_RDWR);
```

---

## 10. Makefile

```makefile
obj-m += mychardev_ioctl.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc test_ioctl.c -o test_ioctl

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f test_ioctl
```

Makefile build:

```text
mychardev_ioctl.c
        |
        v
mychardev_ioctl.ko
```

và:

```text
test_ioctl.c
        |
        v
test_ioctl
```

---

## 11. Build Bài 4

```bash
make
```

Kiểm tra:

```bash
ls -lh mychardev_ioctl.ko
```

Nếu file `.ko` xuất hiện thì build thành công.

---

## 12. Load module

```bash
sudo insmod mychardev_ioctl.ko
```

Kiểm tra:

```bash
lsmod | grep mychardev
```

Ví dụ:

```text
mychardev_ioctl    12288    0
```

Giải thích:

```text
mychardev_ioctl    12288    0
      |              |      |
      |              |      +--> Used by
      |              +---------> Size module
      +------------------------> Tên module
```

---

## 13. Phân biệt Module và Device Node

### Kernel Module

```text
mychardev_ioctl
```

Kiểm tra:

```bash
lsmod | grep mychardev_ioctl
```

Unload:

```bash
sudo rmmod mychardev_ioctl
```

### Device Node

```text
/dev/mychardev
```

Kiểm tra:

```bash
ls -l /dev/mychardev
```

Ví dụ:

```text
crw------- 1 root root 235, 0 ... /dev/mychardev
```

Trong đó:

```text
c   = Character Device
235 = Major Number
0   = Minor Number
```

---

## 14. Cấp quyền test

```bash
sudo chmod 666 /dev/mychardev
```

Kết quả:

```text
crw-rw-rw-
```

---

## 15. Ghi dữ liệu vào Driver

```bash
echo -n "ABCDEFG" > /dev/mychardev
```

Dữ liệu có:

```text
7 byte
```

Luồng:

```text
echo
   |
   v
write()
   |
   v
my_write()
   |
   v
copy_from_user()
   |
   v
kernel_buffer = "ABCDEFG"
   |
   v
data_size = 7
```

---

## 16. Đọc dữ liệu

```bash
cat /dev/mychardev
```

Kết quả:

```text
ABCDEFG
```

---

## 17. Test `ioctl()`

Chạy:

```bash
./test_ioctl
```

Kết quả mong đợi:

```text
Device opened successfully
Buffer length = 7 bytes
Clearing buffer...
Buffer length after clear = 0 bytes
```

Luồng:

```text
kernel_buffer = "ABCDEFG"
data_size = 7

        |
        | IOCTL_GET_LEN
        v

len = 7

        |
        | IOCTL_CLEAR
        v

kernel_buffer = rỗng
data_size = 0

        |
        | IOCTL_GET_LEN
        v

len = 0
```

---

## 18. Kiểm tra Kernel Log

```bash
sudo dmesg | tail -n 20
```

Có thể thấy:

```text
mychardev: loading ioctl module
mychardev: ioctl module loaded successfully
mychardev: wrote 7 bytes
mychardev: buffer length = 7
mychardev: buffer cleared
mychardev: buffer length = 0
mychardev: device closed
```

---

## 19. Unload module

Tên module là:

```text
mychardev_ioctl
```

nên dùng:

```bash
sudo rmmod mychardev_ioctl
```

Không dùng:

```bash
sudo rmmod mychardev
```

vì `mychardev` là tên Device Node.

---

## 20. Kiểm tra unload thành công

### Kiểm tra module

```bash
lsmod | grep mychardev
```

Nếu không có output thì module đã được unload.

### Kiểm tra Device Node

```bash
ls -l /dev/mychardev
```

Kết quả mong đợi:

```text
No such file or directory
```

### Kiểm tra Major Number

```bash
cat /proc/devices | grep mychardev
```

Không có output.

### Kiểm tra log

```bash
sudo dmesg | tail -n 10
```

Kết quả mong đợi:

```text
mychardev: ioctl module unloaded
```

---

## 21. Các lệnh chính

| Lệnh | Chức năng |
|---|---|
| `make` | Build Kernel Module và chương trình test |
| `sudo insmod mychardev_ioctl.ko` | Load module vào Kernel |
| `lsmod \| grep mychardev` | Kiểm tra module |
| `ls -l /dev/mychardev` | Kiểm tra Device Node |
| `sudo chmod 666 /dev/mychardev` | Cấp quyền test |
| `echo -n "ABCDEFG" > /dev/mychardev` | Ghi dữ liệu |
| `cat /dev/mychardev` | Đọc dữ liệu |
| `./test_ioctl` | Test `ioctl()` |
| `sudo dmesg \| tail -n 20` | Xem Kernel Log |
| `sudo rmmod mychardev_ioctl` | Unload module |
| `cat /proc/devices \| grep mychardev` | Kiểm tra Character Device |

---

## 22. Kết quả cần đạt

```text
mychardev_ioctl.c
        |
        v
make
        |
        v
mychardev_ioctl.ko
        |
        v
insmod
        |
        v
/dev/mychardev
        |
        +-----------------------------+
        |                             |
        v                             v
read / write                       ioctl
        |                             |
        v                             v
kernel_buffer                  my_ioctl()
                                      |
                         +------------+------------+
                         |                         |
                         v                         v
                  IOCTL_GET_LEN              IOCTL_CLEAR
```

### Checklist

- [x] Tạo `mychardev_ioctl.c`
- [x] Tạo `test_ioctl.c`
- [x] Tạo `Makefile`
- [ ] Build được `mychardev_ioctl.ko`
- [ ] Load module bằng `insmod`
- [ ] Kiểm tra `/dev/mychardev`
- [ ] Ghi `"ABCDEFG"` vào Driver
- [ ] Test `IOCTL_GET_LEN`
- [ ] Test `IOCTL_CLEAR`
- [ ] Kiểm tra Kernel Log
- [ ] Unload module thành công

---

## 23. Kết luận

Bài 4 mở rộng Character Device Driver bằng cơ chế `ioctl()`.

```text
read()
    |
    v
Đọc dữ liệu

write()
    |
    v
Ghi dữ liệu

ioctl()
    |
    v
Điều khiển Driver
```

Trong bài:

```text
IOCTL_GET_LEN
    |
    v
Lấy độ dài buffer

IOCTL_CLEAR
    |
    v
Xóa buffer
```

Luồng tổng quát:

```text
User Space
    |
    | ioctl(fd, command, ...)
    v
Linux Kernel
    |
    v
file_operations
    |
    v
.unlocked_ioctl
    |
    v
my_ioctl()
    |
    v
Driver xử lý command
```
