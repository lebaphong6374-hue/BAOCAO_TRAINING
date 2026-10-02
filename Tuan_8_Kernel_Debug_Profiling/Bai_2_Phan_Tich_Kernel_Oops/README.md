# BÀI 2 — PHÂN TÍCH KERNEL OOPS DUMP

> **Tuần 8 — Kernel Debug & Profiling**  
> Mục tiêu: đọc và phân tích Kernel Oops dump, xác định loại lỗi, process liên quan, function gây lỗi, module và call trace.

---

## 1. Cấu trúc thư mục

| File | Vai trò |
|---|---|
| `oops_dump.txt` | Chứa Kernel Oops dump mẫu |
| `oops_analysis.txt` | Ghi kết quả phân tích |
| `README.md` | Mô tả bài thực hành |

---

## 2. Mục tiêu bài

Sau bài này cần hiểu:

- Kernel Oops là gì.
- `NULL pointer dereference` là gì.
- Ý nghĩa của `#PF`.
- Cách đọc `CPU`, `PID`, `Comm`.
- Cách đọc dòng `RIP`.
- Cách xác định function và module gây lỗi.
- Cách đọc `Call Trace`.
- Cách dùng `grep`, `sed` để lọc thông tin trong dump.
- Phân biệt Kernel Oops và Kernel Panic.

Quy trình:

```text
Kernel Oops
    |
    v
Loai loi
    |
    v
CPU / PID / Comm
    |
    v
RIP
    |
    v
Function loi
    |
    v
Module
    |
    v
Call Trace
    |
    v
Ket luan nguyen nhan
```

---

## 3. Oops dump sử dụng trong bài

File `oops_dump.txt`:

```text
BUG: kernel NULL pointer dereference, address: 0000000000000000
#PF: supervisor write access in kernel mode
#PF: error_code(0x0002) - not-present page

Oops: 0002 [#1] PREEMPT SMP NOPTI

CPU: 2 PID: 5421 Comm: bash
Tainted: G           O       6.8.0-40-generic

RIP: 0010:oops_write+0x24/0x50 [oops_demo]

RSP: 0018:ffffb1234567fdc0
RAX: 0000000000000000
RBX: ffff912345678900
RCX: 0000000000000005
RDX: 0000000000000000

Call Trace:
 <TASK>
 vfs_write+0x120/0x400
 ksys_write+0x70/0xf0
 __x64_sys_write+0x19/0x20
 do_syscall_64+0x82/0x160
 entry_SYSCALL_64_after_hwframe+0x78/0x80
 </TASK>

Modules linked in: oops_demo(OE)

---[ end trace 0000000000000000 ]---
```

---

## 4. Kernel Oops là gì?

Kernel Oops là thông báo cho biết Linux Kernel gặp lỗi nghiêm trọng trong lúc thực thi.

Trong bài này lỗi là:

```text
NULL pointer dereference
```

Kernel có thể vẫn tiếp tục chạy sau một Oops, nhưng trạng thái hệ thống có thể không còn đáng tin cậy.

---

## 5. Phân tích loại lỗi

Dòng:

```text
BUG: kernel NULL pointer dereference, address: 0000000000000000
```

cho biết Kernel đang truy cập một con trỏ `NULL`.

Địa chỉ lỗi:

```text
0x0000000000000000
```

Ví dụ:

```c
int *ptr = NULL;
*ptr = 10;
```

Luồng:

```text
ptr = NULL
   |
   v
*ptr = 10
   |
   v
Truy cap dia chi 0x0
   |
   v
Page Fault
   |
   v
Kernel Oops
```

---

## 6. `dereference` là gì?

Ví dụ:

```c
int a = 10;
int *ptr = &a;
```

`ptr` giữ địa chỉ của `a`.

Khi dùng:

```c
*ptr
```

ta truy cập dữ liệu tại địa chỉ mà `ptr` trỏ tới. Đó gọi là **dereference**.

Nếu:

```c
ptr = NULL;
```

mà vẫn truy cập:

```c
*ptr
```

thì sẽ truy cập địa chỉ không hợp lệ.

---

## 7. Phân tích Page Fault

Dòng:

```text
#PF: supervisor write access in kernel mode
```

Trong đó:

```text
#PF = Page Fault
```

`write access` cho biết Kernel đang cố ghi dữ liệu vào vùng nhớ không hợp lệ.

```text
Kernel
   |
   | write
   v
Dia chi khong hop le
   |
   v
Page Fault
   |
   v
Kernel Oops
```

---

## 8. CPU, PID và process

Dòng:

```text
CPU: 2 PID: 5421 Comm: bash
```

| Thành phần | Giá trị | Ý nghĩa |
|---|---:|---|
| `CPU` | `2` | Lỗi xảy ra trên CPU core 2 |
| `PID` | `5421` | Process ID |
| `Comm` | `bash` | Process đang chạy |

---

## 9. Phân tích RIP

Dòng quan trọng nhất:

```text
RIP: 0010:oops_write+0x24/0x50 [oops_demo]
```

Tách ra:

```text
Function loi : oops_write()
Offset        : +0x24
Module        : oops_demo
```

`RIP` trên x86-64 chứa địa chỉ instruction CPU đang thực thi.

Có thể hiểu:

```text
RIP
= CPU dang chay lenh nao
```

---

## 10. Phân tích Call Trace

Dump:

```text
Call Trace:
 <TASK>
 vfs_write+0x120/0x400
 ksys_write+0x70/0xf0
 __x64_sys_write+0x19/0x20
 do_syscall_64+0x82/0x160
 entry_SYSCALL_64_after_hwframe+0x78/0x80
 </TASK>
```

Luồng gọi có thể hiểu:

```text
User Space
    |
    | write()
    v
entry_SYSCALL_64_after_hwframe
    |
    v
do_syscall_64
    |
    v
__x64_sys_write
    |
    v
ksys_write
    |
    v
vfs_write
    |
    v
oops_write()
    |
    v
NULL pointer dereference
    |
    v
Kernel Oops
```

`RIP` chỉ vị trí hiện tại xảy ra lỗi, còn `Call Trace` cho biết đường gọi dẫn tới vị trí đó.

---

## 11. Module liên quan

Dòng:

```text
Modules linked in: oops_demo(OE)
```

Module liên quan là:

```text
oops_demo
```

---

## 12. Kết quả phân tích

| Nội dung | Kết quả |
|---|---|
| Loại lỗi | `NULL pointer dereference` |
| Địa chỉ lỗi | `0x0000000000000000` |
| Memory fault | Write access vào vùng nhớ không hợp lệ |
| CPU | `CPU 2` |
| PID | `5421` |
| Process | `bash` |
| Function lỗi | `oops_write()` |
| Offset | `+0x24` |
| Module | `oops_demo` |
| Nguyên nhân có thể | Dereference con trỏ `NULL` |

---

## 13. Nội dung `oops_analysis.txt`

```text
PHAN TICH KERNEL OOPS
=====================

1. Loai loi:
NULL pointer dereference

2. Dia chi loi:
0x0000000000000000

3. Memory fault:
Kernel thuc hien write access vao dia chi bo nho khong hop le.

4. CPU:
CPU 2

5. Process:
PID: 5421
Comm: bash

6. Function loi:
oops_write()

7. Offset:
+0x24

8. Module:
oops_demo

9. Call path:

write()
  |
  v
entry_SYSCALL_64_after_hwframe
  |
  v
do_syscall_64
  |
  v
__x64_sys_write
  |
  v
ksys_write
  |
  v
vfs_write
  |
  v
oops_write()
  |
  v
NULL pointer dereference
  |
  v
Kernel Oops

10. Ket luan:

Driver oops_demo co the da truy cap mot con tro NULL
trong ham oops_write(), dan den Page Fault va Kernel Oops.
```

---

## 14. Dùng `grep` để lọc dump

Tìm loại lỗi:

```bash
grep "BUG:" oops_dump.txt
```

Kết quả:

```text
BUG: kernel NULL pointer dereference, address: 0000000000000000
```

Tìm CPU, PID và process:

```bash
grep "CPU:" oops_dump.txt
```

Kết quả:

```text
CPU: 2 PID: 5421 Comm: bash
```

Tìm function lỗi:

```bash
grep "RIP:" oops_dump.txt
```

Kết quả:

```text
RIP: 0010:oops_write+0x24/0x50 [oops_demo]
```

Tìm module:

```bash
grep "Modules linked" oops_dump.txt
```

Kết quả:

```text
Modules linked in: oops_demo(OE)
```

---

## 15. Dùng `sed` để lấy Call Trace

```bash
sed -n '/Call Trace:/,/<\/TASK>/p' oops_dump.txt
```

Kết quả:

```text
Call Trace:
 <TASK>
 vfs_write+0x120/0x400
 ksys_write+0x70/0xf0
 __x64_sys_write+0x19/0x20
 do_syscall_64+0x82/0x160
 entry_SYSCALL_64_after_hwframe+0x78/0x80
 </TASK>
```

---

## 16. `faddr2line`

Nếu có module thật `oops_demo.ko`, có thể dùng:

```bash
/lib/modules/$(uname -r)/build/scripts/faddr2line ./oops_demo.ko 'oops_write+0x24/0x50'
```

Mục tiêu:

```text
oops_write+0x24
        |
        v
oops_demo.c:<dong_code>
```

Trong bài hiện tại, dump là dump mẫu và không có `oops_demo.ko` thật, nên chưa thể map chính xác về dòng source thật.

---

## 17. Oops và Panic

| Kernel Oops | Kernel Panic |
|---|---|
| Kernel gặp lỗi nghiêm trọng | Kernel không thể tiếp tục an toàn |
| Hệ thống có thể vẫn tiếp tục chạy | Thường dừng hoặc reboot tùy cấu hình |
| Có Oops dump và Call Trace | Có thể xuất hiện sau lỗi nghiêm trọng |

Có thể nhớ:

```text
Oops
 |
 v
Co the van chay


Panic
 |
 v
Khong the tiep tuc an toan
```

---

## 18. Quy trình chuẩn khi phân tích Oops

```text
1. Tim BUG / loai loi
        |
        v
2. Doc CPU / PID / Comm
        |
        v
3. Tim RIP
        |
        v
4. Xac dinh function
        |
        v
5. Xac dinh module
        |
        v
6. Doc Call Trace
        |
        v
7. Map ve source neu co module that
        |
        v
8. Ket luan nguyen nhan
```

Khi dump rất dài, ưu tiên:

```text
BUG
 |
 v
RIP
 |
 v
Call Trace
 |
 v
Module
```

---

## 19. Checklist

- [x] Tạo `oops_dump.txt`
- [x] Xác định `NULL pointer dereference`
- [x] Xác định địa chỉ lỗi `0x0`
- [x] Xác định CPU
- [x] Xác định PID
- [x] Xác định process
- [x] Xác định RIP
- [x] Xác định function lỗi
- [x] Xác định module
- [x] Đọc Call Trace
- [x] Tạo `oops_analysis.txt`
- [x] Biết dùng `grep`
- [x] Biết dùng `sed`
- [x] Hiểu mục đích của `faddr2line`
- [x] Phân biệt Oops và Panic

---

## 20. Kết luận

Bài 2 đã thực hiện phân tích một Kernel Oops dump có lỗi:

```text
NULL pointer dereference
```

Kết quả chính:

```text
CPU      : 2
PID      : 5421
Process  : bash
Function : oops_write()
Offset   : +0x24
Module   : oops_demo
```

Luồng lỗi:

```text
User Space
    |
    | write()
    v
System Call
    |
    v
VFS
    |
    v
oops_write()
    |
    v
NULL pointer dereference
    |
    v
Page Fault
    |
    v
Kernel Oops
```


