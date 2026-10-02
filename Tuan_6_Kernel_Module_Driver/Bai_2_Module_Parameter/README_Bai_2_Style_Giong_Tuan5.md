# BÀI 2 — KERNEL MODULE PARAMETER

> **Tuần 6 — Kernel Module & Driver**  
> Thực hành truyền tham số từ User Space vào Linux Kernel Module, kiểm tra qua `sysfs`, theo dõi Kernel Log và quản lý module bằng `insmod` / `rmmod`.

---

## 1. Mục tiêu

Bài thực hành nhằm tìm hiểu cơ chế **Module Parameter** trong Linux Kernel.

Sau khi hoàn thành, cần nắm được:

- Khai báo tham số bằng `module_param()`.
- Truyền giá trị cho module khi dùng `insmod`.
- Kiểm tra tham số trong `sysfs`.
- Theo dõi Kernel Log bằng `dmesg`.
- Load / unload module bằng `insmod` và `rmmod`.
- Hiểu các file được sinh ra trong quá trình build.

### Quy trình tổng quát

```text
param.c
   |
   v
Makefile
   |
   v
make
   |
   v
param.ko
   |
   | sudo insmod param.ko count=5
   v
Linux Kernel
   |
   | đọc Module Parameter
   v
count = 5
   |
   v
param_init()
   |
   v
pr_info()
   |
   v
Kernel Log
```

---

## 2. Kiến thức cơ bản

### 2.1. Kernel Module là gì?

Kernel Module là mã có thể được nạp vào hoặc gỡ khỏi Linux Kernel trong lúc hệ thống đang chạy.

```text
param.c
   |
   v
Build
   |
   v
param.ko
   |
   +---- insmod ----> Load vào Kernel
   |
   +---- rmmod -----> Gỡ khỏi Kernel
```

### 2.2. Module Parameter là gì?

Module Parameter cho phép truyền giá trị từ User Space vào Kernel Module mà không cần sửa source code rồi build lại.

Ví dụ:

```bash
sudo insmod param.ko count=5
```

Khi đó:

```text
count = 5
```

sẽ được thiết lập trước khi `param_init()` chạy.

---

## 3. Cấu trúc thư mục

```text
Bai_2_Module_Parameter/
├── param.c
├── Makefile
├── param.ko
├── param.o
├── param.mod
├── param.mod.c
├── param.mod.o
├── Module.symvers
├── modules.order
└── *.cmd
```

| File | Vai trò |
|---|---|
| `param.c` | Source code chính |
| `Makefile` | Quy tắc build |
| `param.o` | Object file |
| `param.ko` | Kernel Module hoàn chỉnh |
| `param.mod.c` | Metadata source do Kernel Build System sinh |
| `param.mod.o` | Object metadata |
| `Module.symvers` | Symbol/version |
| `modules.order` | Thứ tự module build |
| `*.cmd` | Lệnh build và dependency |

---

## 4. Source code `param.c`

Tạo file:

```bash
nano param.c
```

Nội dung:

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int count = 1;

module_param(count, int, 0644);
MODULE_PARM_DESC(count, "Number of hello messages");

static int __init param_init(void)
{
    int i;

    pr_info("param_module: count = %d\n", count);

    for (i = 0; i < count; i++)
        pr_info("Hello Kernel %d\n", i + 1);

    return 0;
}

static void __exit param_exit(void)
{
    pr_info("param_module: goodbye\n");
}

module_init(param_init);
module_exit(param_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nguyen Minh Tuan");
MODULE_DESCRIPTION("Kernel Module Parameter Example");
```

---

## 5. Giải thích Module Parameter

### 5.1. Khai báo biến

```c
static int count = 1;
```

```text
Tên biến      : count
Kiểu dữ liệu  : int
Mặc định      : 1
```

### 5.2. Đăng ký parameter

```c
module_param(count, int, 0644);
```

```text
count  -> tên parameter
int    -> kiểu dữ liệu
0644   -> quyền truy cập trong sysfs
```

Quyền `0644`:

```text
Owner  : rw-
Group  : r--
Other  : r--
```

### 5.3. Mô tả parameter

```c
MODULE_PARM_DESC(count, "Number of hello messages");
```

Có thể xem bằng:

```bash
modinfo param.ko
```

---

## 6. Module init / exit

### Khi load module

```bash
sudo insmod param.ko count=5
```

```text
insmod param.ko count=5
        |
        v
Kernel load module
        |
        v
Kernel đọc parameter
        |
        v
count = 5
        |
        v
param_init()
        |
        v
Hello Kernel 1
Hello Kernel 2
Hello Kernel 3
Hello Kernel 4
Hello Kernel 5
```

### Khi unload module

```bash
sudo rmmod param
```

```text
rmmod
  |
  v
param_exit()
  |
  v
param_module: goodbye
```

---

## 7. Makefile

Tạo file:

```bash
nano Makefile
```

Nội dung:

```makefile
obj-m += param.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
```

### Quy trình build

```text
param.c
   |
   v
param.o
   |
   v
Kernel Build System
   |
   v
param.ko
```

---

## 8. Build module

Chạy:

```bash
make
```

Kiểm tra:

```bash
ls -lh param.ko
```

Nếu file `param.ko` xuất hiện thì build thành công.

---

## 9. Test với giá trị mặc định

Load module:

```bash
sudo insmod param.ko
```

Kiểm tra:

```bash
lsmod | grep param
```

Xem log:

```bash
sudo dmesg | tail -n 10
```

Kết quả mong đợi:

```text
param_module: count = 1
Hello Kernel 1
```

Unload:

```bash
sudo rmmod param
```

---

## 10. Test với `count=5`

Load:

```bash
sudo insmod param.ko count=5
```

Kiểm tra Kernel Log:

```bash
sudo dmesg | tail -n 10
```

Kết quả mong đợi:

```text
param_module: count = 5
Hello Kernel 1
Hello Kernel 2
Hello Kernel 3
Hello Kernel 4
Hello Kernel 5
```

---

## 11. Kiểm tra parameter trong `sysfs`

```bash
cat /sys/module/param/parameters/count
```

Kết quả:

```text
5
```

Quy trình:

```text
module_param()
     |
     v
Kernel đăng ký parameter
     |
     v
/sys/module/param/parameters/count
```

Có thể thay đổi:

```bash
echo 3 | sudo tee /sys/module/param/parameters/count
```

Kiểm tra lại:

```bash
cat /sys/module/param/parameters/count
```

Kết quả:

```text
3
```

> Việc thay đổi `count` trong `sysfs` không làm `param_init()` chạy lại.

---

## 12. Các lệnh sử dụng trong bài

| Lệnh | Chức năng |
|---|---|
| `make` | Build module |
| `make clean` | Xóa file build |
| `sudo insmod param.ko` | Load module |
| `sudo insmod param.ko count=5` | Load module với parameter |
| `sudo rmmod param` | Unload module |
| `lsmod \| grep param` | Kiểm tra module |
| `modinfo param.ko` | Xem metadata |
| `sudo dmesg` | Xem Kernel Log |
| `tail -n 10` | Lấy 10 dòng cuối |
| `grep` | Lọc output theo từ khóa |

---

## 13. Kết quả đạt được

```text
Source Code
   |
   v
Build
   |
   v
param.ko
   |
   v
insmod
   |
   v
Kernel nhận parameter
   |
   v
param_init()
   |
   v
Kernel Log
```

### Checklist

- [x] Tạo `param.c`
- [x] Tạo `Makefile`
- [x] Build được `param.ko`
- [ ] Test `sudo insmod param.ko`
- [ ] Test `sudo insmod param.ko count=5`
- [ ] Kiểm tra `/sys/module/param/parameters/count`
- [ ] Kiểm tra Kernel Log bằng `dmesg`
- [ ] Unload module bằng `rmmod`

---

## 14. Kết luận

Bài thực hành giúp làm rõ cơ chế **Module Parameter** trong Linux Kernel Module.

Module Parameter cho phép truyền cấu hình từ User Space vào Kernel Space ngay tại thời điểm module được load:

```text
User Space
    |
    | insmod param.ko count=5
    v
Kernel Space
    |
    | xử lý parameter
    v
count = 5
    |
    v
param_init()
```

Các nội dung cần ghi nhớ:

```text
module_param()
MODULE_PARM_DESC()
module_init()
module_exit()
insmod
rmmod
lsmod
dmesg
sysfs
```

