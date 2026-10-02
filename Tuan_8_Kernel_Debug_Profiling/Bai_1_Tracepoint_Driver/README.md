# BÀI 1 — THÊM TRACEPOINT VÀO DRIVER TỰ VIẾT

> **Tuần 8 — Kernel Debug & Profiling**  
> Mục tiêu: tích hợp **custom tracepoint** vào Character Device Driver, kích hoạt tracepoint khi `write()` xảy ra và quan sát event thông qua `tracefs`.

---

## 1. Mục tiêu bài thực hành

Sau bài này cần nắm được:

- Cách tạo custom tracepoint bằng `TRACE_EVENT()`.
- Cách tích hợp tracepoint vào driver.
- Cách build và load Kernel Module.
- Cách kiểm tra tracepoint đã được Kernel đăng ký.
- Cách enable / disable tracepoint.
- Cách đọc trace buffer tại `/sys/kernel/tracing/trace`.
- Phân biệt `dmesg` và tracing buffer.

Luồng tổng quát:

```text
User Space
    |
    | write()
    v
/dev/week8_driver
    |
    v
Linux Kernel
    |
    v
week8_write()
    |
    +--> copy_from_user()
    |
    +--> trace_week8_write()
    |
    v
Trace Buffer
    |
    v
/sys/kernel/tracing/trace
```

---

## 2. Cấu trúc thư mục

| File | Vai trò |
|---|---|
| `week8_driver.c` | Character Device Driver |
| `week8_trace.h` | Khai báo custom tracepoint |
| `Makefile` | Build Kernel Module |
| `README.md` | Báo cáo bài thực hành |

---

## 3. File `week8_trace.h`

```c
#undef TRACE_SYSTEM
#define TRACE_SYSTEM week8_driver

#if !defined(_WEEK8_TRACE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _WEEK8_TRACE_H

#include <linux/tracepoint.h>

TRACE_EVENT(week8_write,

    TP_PROTO(size_t len),

    TP_ARGS(len),

    TP_STRUCT__entry(
        __field(size_t, len)
    ),

    TP_fast_assign(
        __entry->len = len;
    ),

    TP_printk("len=%zu", __entry->len)
);

#endif

#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .

#undef TRACE_INCLUDE_FILE
#define TRACE_INCLUDE_FILE week8_trace

#include <trace/define_trace.h>
```

### Ý nghĩa chính

```text
TRACE_SYSTEM
→ tên nhóm trace: week8_driver

TRACE_EVENT
→ tên event: week8_write

TP_PROTO
→ tham số tracepoint nhận vào

TP_STRUCT__entry
→ dữ liệu được lưu trong trace entry

TP_fast_assign
→ gán giá trị runtime

TP_printk
→ cách dữ liệu được hiển thị
```

Event đầy đủ:

```text
week8_driver:week8_write
```

---

## 4. Tích hợp tracepoint vào driver

Trong `week8_driver.c`:

```c
#define CREATE_TRACE_POINTS
#include "week8_trace.h"
```

Trong hàm `week8_write()`:

```c
trace_week8_write(bytes_to_write);
```

Luồng:

```text
write()
   |
   v
week8_write()
   |
   +--> copy_from_user()
   |
   +--> data_size = bytes_to_write
   |
   +--> trace_week8_write(bytes_to_write)
   |
   v
Trace Buffer
```

---

## 5. `file_operations`

```c
static const struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = week8_open,
    .read    = week8_read,
    .write   = week8_write,
    .release = week8_release,
};
```

Ánh xạ:

```text
open()   -> week8_open()
read()   -> week8_read()
write()  -> week8_write()
close()  -> week8_release()
```

Custom tracepoint được kích hoạt bên trong `week8_write()`.

---

## 6. Makefile

```makefile
obj-m += week8_driver.o

ccflags-y += -I$(src)

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
```

`ccflags-y += -I$(src)` giúp compiler tìm file `week8_trace.h`.

---

## 7. Build Kernel Module

Chạy:

```bash
make
```

Kết quả build thực tế:

```text
CC [M]  .../week8_driver.o
MODPOST .../Module.symvers
CC [M]  .../week8_driver.mod.o
LD [M]  .../week8_driver.ko
BTF [M] .../week8_driver.ko
```

Có thông báo:

```text
Skipping BTF generation ... due to unavailability of vmlinux
```

Đây không phải lỗi build.

Kiểm tra:

```bash
ls -lh week8_driver.ko
```

Kết quả thực tế:

```text
-rw-rw-r-- 1 do do 418K ... week8_driver.ko
```

---

## 8. Load module

```bash
sudo insmod week8_driver.ko
```

Kiểm tra:

```bash
lsmod | grep week8
```

Kết quả thực tế:

```text
week8_driver           20480  0
```

---

## 9. Kiểm tra Kernel Log

```bash
sudo dmesg | tail -n 10
```

Kết quả thực tế:

```text
week8_driver: loading module
week8_driver: major=235 minor=0
week8_driver: module loaded successfully
```

`dmesg` dùng để xem Kernel Log.

```text
pr_info() / printk()
        |
        v
Kernel Log Buffer
        |
        v
dmesg
```

---

## 10. Kiểm tra Device Node

```bash
ls -l /dev/week8_driver
```

Kết quả thực tế:

```text
crw------- 1 root root 235, 0 ... /dev/week8_driver
```

Trong đó:

```text
c   = Character Device
235 = Major Number
0   = Minor Number
```

Cấp quyền test:

```bash
sudo chmod 666 /dev/week8_driver
```

---

## 11. Kiểm tra `tracefs`

```bash
sudo ls /sys/kernel/tracing
```

đã thấy các file:

```text
available_events
available_tracers
current_tracer
events
trace
trace_pipe
tracing_on
...
```

Kiểm tra mount:

```bash
mount | grep tracefs
```

Kết quả thực tế:

```text
tracefs on /sys/kernel/tracing type tracefs (rw,nosuid,nodev,noexec,relatime)
```

=> `tracefs` đã được mount đúng.

---

## 12. Kiểm tra custom tracepoint

```bash
sudo grep week8 /sys/kernel/tracing/available_events
```

Kết quả thực tế:

```text
week8_driver:week8_write
```

Điều này chứng minh tracepoint đã được Kernel đăng ký thành công.

---

## 13. Enable tracepoint

```bash
sudo sh -c 'echo 1 > /sys/kernel/tracing/events/week8_driver/week8_write/enable'
```

Ý nghĩa:

```text
1 = enable event
```

Dùng `sudo sh -c` vì phép redirect `>` cũng cần quyền root.

---

## 14. Xóa trace cũ

```bash
sudo sh -c 'echo > /sys/kernel/tracing/trace'
```

Lệnh này chỉ xóa dữ liệu cũ trong trace buffer.

---

## 15. Test lần 1 — `"Hello"`

```bash
echo -n "Hello" > /dev/week8_driver
```

`Hello` có:

```text
5 byte
```

Xem trace:

```bash
sudo cat /sys/kernel/tracing/trace
```

Kết quả thực tế:

```text
bash-3142386 [014] ..... 613036.537797: week8_write: len=5
```

---

## 16. Phân tích dòng trace

```text
bash-3142386 [014] ..... 613036.537797: week8_write: len=5
```

| Thành phần | Ý nghĩa |
|---|---|
| `bash` | Process gây ra event |
| `3142386` | PID |
| `[014]` | CPU core 14 |
| `613036.537797` | Timestamp |
| `week8_write` | Tên tracepoint |
| `len=5` | Số byte ghi vào driver |

---

## 17. Test lần 2 — `"BSP Training Week 8"`

```bash
echo -n "BSP Training Week 8" > /dev/week8_driver
```

Chuỗi có:

```text
19 byte
```

Xem:

```bash
sudo tail -n 10 /sys/kernel/tracing/trace
```

Kết quả thực tế:

```text
bash-3142386 [014] ..... 613036.537797: week8_write: len=5
bash-3142386 [014] ..... 613338.777765: week8_write: len=19
```

Điều này chứng minh tracepoint nhận đúng dữ liệu runtime.

---

## 18. `# tracer: nop`

Trong trace có:

```text
# tracer: nop
```

Đây không phải lỗi.

`nop` nghĩa là chưa bật function tracer như:

```text
function
function_graph
```

Bài này dùng **custom tracepoint event**, nên event vẫn hoạt động bình thường.

---

## 19. Disable tracepoint

```bash
sudo sh -c 'echo 0 > /sys/kernel/tracing/events/week8_driver/week8_write/enable'
```

Kiểm tra bằng:

```bash
sudo cat /sys/kernel/tracing/events/week8_driver/week8_write/enable
```

Kết quả mong đợi:

```text
0
```

Nếu dùng `cat` không có `sudo`, hệ thống có thể báo:

```text
Permission denied
```

---

## 20. Unload module

Sau khi disable tracepoint:

```bash
sudo rmmod week8_driver
```

Kiểm tra:

```bash
lsmod | grep week8
```

Không có output nghĩa là module đã được unload.

Kiểm tra Device Node:

```bash
ls -l /dev/week8_driver
```

Kết quả:

```text
No such file or directory
```

Kiểm tra log:

```bash
sudo dmesg | tail -n 10
```

Kết quả:

```text
week8_driver: module unloaded
```

---

## 21. Các lệnh chính

| Lệnh | Chức năng |
|---|---|
| `make` | Build Kernel Module |
| `sudo insmod week8_driver.ko` | Load module |
| `lsmod \| grep week8` | Kiểm tra module |
| `sudo dmesg \| tail -n 10` | Xem Kernel Log |
| `ls -l /dev/week8_driver` | Kiểm tra device node |
| `mount \| grep tracefs` | Kiểm tra tracefs |
| `sudo grep week8 /sys/kernel/tracing/available_events` | Kiểm tra tracepoint |
| `echo 1 > .../enable` | Enable tracepoint |
| `echo 0 > .../enable` | Disable tracepoint |
| `sudo cat /sys/kernel/tracing/trace` | Xem trace |
| `sudo rmmod week8_driver` | Unload module |

---

## 22. Kết quả đạt được

```text
week8_driver.ko
→ 418K

week8_driver
→ đã load vào Kernel

/dev/week8_driver
→ Character Device
→ Major 235
→ Minor 0

Custom event
→ week8_driver:week8_write

Trace test 1
→ len=5

Trace test 2
→ len=19
```

---

## 23. Checklist

- [x] Tạo `week8_driver.c`
- [x] Tạo `week8_trace.h`
- [x] Tạo `Makefile`
- [x] Build được `week8_driver.ko`
- [x] Load module thành công
- [x] Tạo `/dev/week8_driver`
- [x] Kiểm tra `tracefs`
- [x] Custom tracepoint được đăng ký
- [x] Enable tracepoint
- [x] Trace `"Hello"` → `len=5`
- [x] Trace `"BSP Training Week 8"` → `len=19`
- [x] Disable tracepoint
- [ ] Xác nhận unload module hoàn tất

---

## 24.  Kết luận

Bài 1 đã tích hợp thành công custom tracepoint vào Character Device Driver.

Luồng hoàn chỉnh:

```text
User Space
    |
    | write()
    v
/dev/week8_driver
    |
    v
week8_write()
    |
    +--> copy_from_user()
    |
    +--> trace_week8_write()
    |
    v
Trace Buffer
    |
    v
/sys/kernel/tracing/trace
```

Kết quả thực tế:

```text
"Hello"
   |
   v
len=5

"BSP Training Week 8"
   |
   v
len=19
```

Qua bài này đã phân biệt được:

```text
printk / pr_info
        |
        v
dmesg


TRACE_EVENT
        |
        v
tracefs
        |
        v
/sys/kernel/tracing/trace
```
