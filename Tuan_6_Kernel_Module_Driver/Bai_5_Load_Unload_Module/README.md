# BÀI 5 — LOAD / UNLOAD KERNEL MODULE

> **Tuần 6 — Linux Kernel Module & Driver**  
> Bài thực hành tập trung vào **vòng đời của Kernel Module**: build module, load vào Kernel, kiểm tra trạng thái hoạt động, unload module và xác nhận module đã được gỡ hoàn toàn.

---

## 1. Mục tiêu

Sau khi hoàn thành bài này cần nắm được:

- Cách build source thành file Kernel Module `.ko`.
- Cách load module bằng `insmod`.
- Cách kiểm tra module bằng `lsmod`.
- Cách xem metadata bằng `modinfo`.
- Cách kiểm tra module trong `/sys/module`.
- Cách unload module bằng `rmmod`.
- Cách xác nhận module đã được gỡ khỏi Kernel.
- Hiểu rõ vòng đời:

```text
Source Code
    |
    v
Build
    |
    v
module.ko
    |
    | insmod
    v
Linux Kernel
    |
    | module_init()
    v
Module hoạt động
    |
    | rmmod
    v
module_exit()
    |
    v
Module được gỡ khỏi Kernel
```

---

## 2. Kiến thức cơ bản

### 2.1. Kernel Module là gì?

Kernel Module là một đoạn mã có thể được **nạp vào** hoặc **gỡ khỏi** Linux Kernel trong khi hệ thống vẫn đang chạy.

```text
Linux đang chạy
     |
     +----> Load module
     |
     +----> Module hoạt động
     |
     +----> Unload module
     |
     v
Linux vẫn tiếp tục chạy
```

Không cần:

```text
reboot
shutdown
build lại toàn bộ Kernel
```

---

### 2.2. Vì sao gọi là Load / Unload động?

Bởi vì module có thể được thêm hoặc gỡ khi Kernel đang hoạt động.

```text
lifecycle.ko
     |
     | insmod
     v
Kernel
     |
     | rmmod
     v
Module bị gỡ
```

---

## 3. Cấu trúc thư mục

```text
Bai_5_Load_Unload_Module/
├── lifecycle.c
├── Makefile
├── lifecycle.ko
├── lifecycle.o
├── lifecycle.mod
├── lifecycle.mod.c
├── lifecycle.mod.o
├── Module.symvers
├── modules.order
└── *.cmd
```

| File | Vai trò |
|---|---|
| `lifecycle.c` | Source code Kernel Module |
| `Makefile` | Quy tắc build |
| `lifecycle.o` | Object file |
| `lifecycle.ko` | Kernel Module hoàn chỉnh |
| `lifecycle.mod.c` | Metadata source do Kernel Build System sinh |
| `Module.symvers` | Symbol/version information |
| `modules.order` | Thứ tự module build |
| `*.cmd` | Build command và dependency |

---

## 4. Source code `lifecycle.c`

Tạo file:

```bash
nano lifecycle.c
```

Nội dung:

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init lifecycle_init(void)
{
    pr_info("lifecycle: module loaded into kernel\n");

    return 0;
}

static void __exit lifecycle_exit(void)
{
    pr_info("lifecycle: module removed from kernel\n");
}

module_init(lifecycle_init);
module_exit(lifecycle_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nguyen Minh Tuan");
MODULE_DESCRIPTION("Kernel Module Load Unload Example");
MODULE_VERSION("1.0");
```

---

## 5. Giải thích `module_init()` và `module_exit()`

### Khi module được load

```c
module_init(lifecycle_init);
```

Kernel sẽ gọi:

```c
lifecycle_init()
```

khi chạy:

```bash
sudo insmod lifecycle.ko
```

Luồng:

```text
insmod lifecycle.ko
        |
        v
Linux Kernel
        |
        v
lifecycle_init()
        |
        v
pr_info()
        |
        v
Kernel Log
```

---

### Khi module được unload

```c
module_exit(lifecycle_exit);
```

Kernel sẽ gọi:

```c
lifecycle_exit()
```

khi chạy:

```bash
sudo rmmod lifecycle
```

Luồng:

```text
rmmod lifecycle
       |
       v
Linux Kernel
       |
       v
lifecycle_exit()
       |
       v
Kernel Log
```

---

## 6. Makefile

Tạo file:

```bash
nano Makefile
```

Nội dung:

```makefile
obj-m += lifecycle.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
```

### Quy trình build

```text
lifecycle.c
     |
     v
lifecycle.o
     |
     v
Kernel Build System
     |
     v
lifecycle.ko
```

---

## 7. Build Kernel Module

Chạy:

```bash
make
```

Kiểm tra:

```bash
ls -lh lifecycle.ko
```

Nếu file tồn tại thì build thành công.

> `make` chỉ build module thành file `.ko`, chưa load module vào Kernel.

---

## 8. Xem thông tin module với `modinfo`

Chạy:

```bash
modinfo lifecycle.ko
```

Có thể xem:

```text
filename
version
description
author
license
vermagic
```

Ví dụ:

```text
description: Kernel Module Load Unload Example
author:      Nguyen Minh Tuan
license:     GPL
version:     1.0
```

`modinfo` chỉ đọc metadata của file `.ko`.

```text
modinfo
   |
   v
Đọc thông tin module

Không load module vào Kernel
```

---

## 9. Kiểm tra trước khi load

Chạy:

```bash
lsmod | grep lifecycle
```

Giải thích:

```text
lsmod
  |
  v
Liệt kê các module đang được load
  |
  | pipe
  v
grep lifecycle
  |
  v
Tìm riêng module lifecycle
```

Nếu chưa load:

```text
không có output
```

---

## 10. Load module vào Kernel

Chạy:

```bash
sudo insmod lifecycle.ko
```

Ý nghĩa:

```text
insmod
= insert module
= nạp module vào Linux Kernel
```

Sau đó kiểm tra:

```bash
lsmod | grep lifecycle
```

Kết quả ví dụ:

```text
lifecycle    12288    0
```

Giải thích:

```text
lifecycle    12288    0
    |          |      |
    |          |      +--> Used by
    |          +---------> Size module
    +--------------------> Tên module
```

---

## 11. Kiểm tra Kernel Log

Chạy:

```bash
sudo dmesg | tail -n 10
```

Kết quả mong đợi:

```text
lifecycle: module loaded into kernel
```

Điều này chứng minh:

```text
lifecycle_init()
```

đã được thực thi.

---

## 12. Kiểm tra module trong `/sys/module`

Khi module đang được load:

```bash
ls /sys/module/lifecycle
```

Có thể thấy:

```text
coresize
holders
initsize
initstate
notes
refcnt
sections
srcversion
taint
uevent
```

Điều này cho thấy Kernel đang quản lý module tại:

```text
/sys/module/lifecycle
```

Có thể kiểm tra trạng thái:

```bash
cat /sys/module/lifecycle/initstate
```

hoặc reference count:

```bash
cat /sys/module/lifecycle/refcnt
```

---

## 13. Unload module

Chạy:

```bash
sudo rmmod lifecycle
```

Ý nghĩa:

```text
rmmod
= remove module
= gỡ module khỏi Kernel
```

Lưu ý:

```text
insmod
→ dùng tên file .ko

rmmod
→ dùng tên module
```

Ví dụ:

```bash
sudo insmod lifecycle.ko
sudo rmmod lifecycle
```

---

## 14. Kiểm tra unload thành công

### 14.1. Kiểm tra bằng `lsmod`

```bash
lsmod | grep lifecycle
```

Nếu không có output:

```text
lifecycle không còn trong Kernel
```

---

### 14.2. Kiểm tra `/sys/module`

```bash
ls /sys/module/lifecycle
```

Kết quả mong đợi:

```text
No such file or directory
```

Điều này chứng minh Kernel không còn quản lý module đó.

---

### 14.3. Kiểm tra Kernel Log

```bash
sudo dmesg | tail -n 10
```

Kết quả mong đợi:

```text
lifecycle: module removed from kernel
```

Điều này chứng minh:

```text
lifecycle_exit()
```

đã được gọi.

---

## 15. Test Load / Unload nhiều lần

Chạy:

```bash
sudo insmod lifecycle.ko
sudo rmmod lifecycle

sudo insmod lifecycle.ko
sudo rmmod lifecycle

sudo insmod lifecycle.ko
sudo rmmod lifecycle
```

Sau đó:

```bash
sudo dmesg | tail -n 20
```

Kết quả mong đợi:

```text
lifecycle: module loaded into kernel
lifecycle: module removed from kernel
lifecycle: module loaded into kernel
lifecycle: module removed from kernel
lifecycle: module loaded into kernel
lifecycle: module removed from kernel
```

Điều này chứng minh module có thể được load/unload động mà không cần reboot.

---

## 16. Test lỗi khi unload hai lần

Sau khi:

```bash
sudo rmmod lifecycle
```

chạy lại:

```bash
sudo rmmod lifecycle
```

Có thể nhận:

```text
rmmod: ERROR: Module lifecycle is not currently loaded
```

Ý nghĩa:

```text
Module đã được unload trước đó
```

---

## 17. Test lỗi khi load hai lần

Load:

```bash
sudo insmod lifecycle.ko
```

rồi load lại:

```bash
sudo insmod lifecycle.ko
```

Có thể nhận:

```text
insmod: ERROR: could not insert module lifecycle.ko: File exists
```

Ý nghĩa:

```text
Module lifecycle đã tồn tại trong Kernel
```

---

## 18. Phân biệt file `.ko` và module trong Kernel

File:

```text
lifecycle.ko
```

chỉ là file nằm trên ổ đĩa.

```text
lifecycle.c
    |
    | make
    v
lifecycle.ko
```

Muốn module hoạt động:

```text
lifecycle.ko
    |
    | insmod
    v
Linux Kernel
```

Do đó:

```text
File .ko tồn tại
        ≠
Module đang được load
```

Muốn kiểm tra module đang trong Kernel:

```bash
lsmod | grep lifecycle
```

---

## 19. Các lệnh chính

| Lệnh | Chức năng |
|---|---|
| `make` | Build module |
| `ls -lh lifecycle.ko` | Kiểm tra file `.ko` |
| `modinfo lifecycle.ko` | Xem metadata |
| `sudo insmod lifecycle.ko` | Load module |
| `lsmod \| grep lifecycle` | Kiểm tra module đang load |
| `sudo dmesg \| tail -n 10` | Xem Kernel Log |
| `ls /sys/module/lifecycle` | Kiểm tra module trong sysfs |
| `sudo rmmod lifecycle` | Unload module |
| `make clean` | Xóa file build |

---

## 20. Quy trình hoàn chỉnh

```text
lifecycle.c
     |
     v
make
     |
     v
lifecycle.ko
     |
     | sudo insmod lifecycle.ko
     v
Linux Kernel
     |
     v
lifecycle_init()
     |
     v
Module hoạt động
     |
     | sudo rmmod lifecycle
     v
lifecycle_exit()
     |
     v
Module bị gỡ khỏi Kernel
```

---

## 21. Checklist

- [x] Tạo `lifecycle.c`
- [x] Tạo `Makefile`
- [ ] Build được `lifecycle.ko`
- [ ] Kiểm tra metadata bằng `modinfo`
- [ ] Load module bằng `insmod`
- [ ] Kiểm tra bằng `lsmod`
- [ ] Kiểm tra `/sys/module/lifecycle`
- [ ] Kiểm tra Kernel Log
- [ ] Unload module bằng `rmmod`
- [ ] Xác nhận module đã được gỡ hoàn toàn
- [ ] Test load/unload nhiều lần

---

## 22. Kết luận

Bài 5 giúp làm rõ **vòng đời của Linux Kernel Module**.

Ba lệnh quan trọng nhất:

```text
make
= build module

insmod
= load module vào Kernel

rmmod
= unload module khỏi Kernel
```

Luồng cần nhớ:

```text
Source
  |
  v
make
  |
  v
.ko
  |
  v
insmod
  |
  v
Kernel
  |
  v
rmmod
  |
  v
Module được gỡ
```

Sau Bài 5, toàn bộ Tuần 6 có thể tóm tắt:

```text
Bài 1
Hello Kernel Module
        |
        v
Bài 2
Module Parameter
        |
        v
Bài 3
Character Device Driver
read / write
        |
        v
Bài 4
ioctl()
        |
        v
Bài 5
Load / Unload Module
```
