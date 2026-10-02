# BÀI 3 — DÙNG `perf stat` ĐỂ ĐO CPU EVENTS

> **Tuần 8 — Kernel Debug & Profiling**  
> Mục tiêu: sử dụng `perf stat` để đo các sự kiện hiệu năng CPU như `cycles`, `instructions`, `branches`, `branch-misses`, `cache-misses`, `context-switches` và `page-faults`.

---

## 1. Cấu trúc thư mục


| File | Vai trò |
|---|---|
| `cpu_test.c` | Chương trình tạo workload cho CPU |
| `README.md` | Báo cáo bài thực hành |

---

## 2. Mục tiêu bài thực hành

Sau bài này cần nắm được:

- `perf` là công cụ gì.
- `perf stat` dùng để làm gì.
- Cách tạo workload đơn giản cho CPU.
- Cách đo các hardware/software event.
- Ý nghĩa của `cycles`, `instructions`, IPC.
- Ý nghĩa của `branches`, `branch-misses`.
- Ý nghĩa của `cache-references`, `cache-misses`.
- Ý nghĩa của `context-switches`, `page-faults`.
- Cách xử lý lỗi quyền truy cập `perf`.

Luồng tổng quát:

```text
cpu_test.c
    |
    | gcc -O2
    v
cpu_test
    |
    v
CPU chạy workload
    |
    +--> cycles
    +--> instructions
    +--> branches
    +--> branch-misses
    +--> cache events
    +--> context-switches
    +--> page-faults
    |
    v
perf stat
    |
    v
Performance Statistics
```

---

## 3. Chương trình `cpu_test.c`

```c
#include <stdio.h>

int main(void)
{
    volatile unsigned long long sum = 0;
    unsigned long long i;

    for (i = 0; i < 200000000ULL; i++)
    {
        sum += i;
    }

    printf("sum = %llu\n", sum);

    return 0;
}
```

---

## 4. Compile chương trình

```bash
gcc -O2 cpu_test.c -o cpu_test
```

Giải thích:

```text
gcc
→ compiler

-O2
→ bật tối ưu mức 2

cpu_test.c
→ source code

-o cpu_test
→ tạo executable tên cpu_test
```

---

## 5. Chạy chương trình

```bash
./cpu_test
```

Kết quả thực tế:

```text
sum = 19999999900000000
```

Điều này xác nhận chương trình chạy đúng trước khi đo bằng `perf`.

---

## 6. Lỗi quyền khi chạy `perf`

Ban đầu chạy:

```bash
perf stat ./cpu_test
```

nhận:

```text
Access to performance monitoring and observability operations is limited.
```

và:

```text
perf_event_paranoid setting is 4
```

Nguyên nhân là user thường bị hạn chế truy cập performance monitoring.

Cách xử lý trong bài:

```bash
sudo perf stat ./cpu_test
```

---

## 7. Kết quả `perf stat` mặc định

Lệnh:

```bash
sudo perf stat ./cpu_test
```

Kết quả thực tế:

```text
sum = 19999999900000000

 Performance counter stats for './cpu_test':

            522,44 msec task-clock                       #    0,999 CPUs utilized
                 1      context-switches                 #    1,914 /sec
                 0      cpu-migrations                   #    0,000 /sec
                58      page-faults                      #  111,018 /sec
     1.356.418.178      cycles                           #    2,596 GHz
     1.204.211.767      instructions                     #    0,89  insn per cycle
       200.705.116      branches                         #  384,170 M/sec
            14.986      branch-misses                    #    0,01% of all branches

       0,523171001 seconds time elapsed

       0,522322000 seconds user
       0,001000000 seconds sys
```

---

## 8. Phân tích kết quả lần 1

### `task-clock`

```text
522,44 msec task-clock
```

CPU thực sự dành khoảng 0,522 giây cho chương trình.

```text
0,999 CPUs utilized
```

cho thấy chương trình gần như sử dụng liên tục một CPU core.

### `context-switches`

```text
1 context-switches
```

Chương trình chỉ bị chuyển task rất ít.

### `cpu-migrations`

```text
0 cpu-migrations
```

Process không bị chuyển sang CPU core khác trong lần đo.

### `page-faults`

```text
58 page-faults
```

Có 58 page fault trong quá trình chạy. Đây không nhất thiết là lỗi nghiêm trọng; với user-space đây thường là hoạt động bình thường của virtual memory.

### `cycles`

```text
1.356.418.178 cycles
```

CPU sử dụng khoảng 1,356 tỷ cycle.

### `instructions`

```text
1.204.211.767 instructions
```

CPU thực thi khoảng 1,204 tỷ machine instruction.

### IPC

Perf hiển thị:

```text
0,89 insn per cycle
```

Công thức:

```text
IPC = Instructions / Cycles
```

Kết quả:

```text
IPC ≈ 0,89
```

### `branches`

```text
200.705.116 branches
```

Có khoảng 200,7 triệu branch, phù hợp với vòng lặp 200 triệu lần.

### `branch-misses`

```text
14.986 branch-misses
```

Perf hiển thị:

```text
0,01% of all branches
```

Branch predictor dự đoán rất tốt vì vòng lặp có kiểu rẽ nhánh đều đặn.

---

## 9. Đo các event cụ thể

Lệnh:

```bash
sudo perf stat \
-e cycles,instructions,branches,branch-misses,cache-references,cache-misses,context-switches,page-faults \
./cpu_test
```

Kết quả thực tế:

```text
sum = 19999999900000000

 Performance counter stats for './cpu_test':

     1.497.860.794      cycles                                                                  (66,58%)
     1.204.905.527      instructions                     #    0,80  insn per cycle              (83,29%)
       200.863.914      branches                                                                (83,29%)
            17.906      branch-misses                    #    0,01% of all branches             (83,32%)
            64.864      cache-references                                                        (83,46%)
             8.184      cache-misses                     #   12,62% of all cache refs           (83,35%)
                 0      context-switches
                58      page-faults

       0,593689445 seconds time elapsed

       0,592753000 seconds user
       0,001001000 seconds sys
```

---

## 10. Bảng kết quả chính

| Event | Kết quả |
|---|---:|
| `cycles` | 1.497.860.794 |
| `instructions` | 1.204.905.527 |
| IPC | 0,80 insn/cycle |
| `branches` | 200.863.914 |
| `branch-misses` | 17.906 |
| Branch miss rate | 0,01% |
| `cache-references` | 64.864 |
| `cache-misses` | 8.184 |
| Cache miss rate | 12,62% |
| `context-switches` | 0 |
| `page-faults` | 58 |
| Time elapsed | 0,593689445 s |

---

## 11. Cache events

`cache-references`:

```text
64.864
```

`cache-misses`:

```text
8.184
```

Perf tính:

```text
12,62% of all cache refs
```

Có thể hiểu:

```text
CPU cần dữ liệu
      |
      v
Cache
      |
      +--> HIT
      |
      +--> MISS
            |
            v
         8.184 lần
```

Lưu ý: đây là hardware event tổng quát do CPU/perf cung cấp, không đồng nghĩa trực tiếp với toàn bộ truy cập L1/L2/L3.

---

## 12. Multiplexing trong `perf`

Trong kết quả có:

```text
(66,58%)
(83,29%)
(83,46%)
```

Ví dụ:

```text
1.497.860.794 cycles (66,58%)
```

Đây không phải phần trăm cycles.

Nguyên nhân:

```text
Yêu cầu nhiều hardware event cùng lúc
        |
        v
CPU có số lượng hardware counter giới hạn
        |
        v
perf phải luân phiên các event
        |
        v
multiplexing
```

Perf sau đó scale kết quả để ước lượng cho toàn bộ khoảng đo.

---

## 13. Vì sao hai lần chạy khác nhau?

Lần 1:

```text
IPC = 0,89
Time = 0,523 s
```

Lần 2:

```text
IPC = 0,80
Time = 0,594 s
```

Điều này là bình thường vì kết quả có thể bị ảnh hưởng bởi:

```text
CPU frequency
scheduler
background process
cache state
interrupt
perf multiplexing
```

Benchmark thực tế thường được chạy nhiều lần để lấy số đại diện.

---

## 14. Nhận xét kết quả

Chương trình `cpu_test` thực hiện vòng lặp 200 triệu lần để tạo workload cho CPU.

Kết quả cho thấy:

```text
Instructions
≈ 1,2 tỷ

Branches
≈ 200 triệu

Branch miss rate
≈ 0,01%

IPC
≈ 0,80 đến 0,89 instruction/cycle

Cache miss rate
≈ 12,62%

Page faults
= 58
```

Số lượng branch gần với số vòng lặp cho thấy vòng `for` là nguồn branch chính. Branch-miss rất thấp vì kiểu rẽ nhánh của vòng lặp rất đều đặn.

---

## 15. Các lệnh chính

| Lệnh | Chức năng |
|---|---|
| `gcc -O2 cpu_test.c -o cpu_test` | Compile chương trình |
| `./cpu_test` | Chạy workload |
| `perf --version` | Kiểm tra perf |
| `perf stat ./cpu_test` | Đo performance mặc định |
| `sudo perf stat ./cpu_test` | Chạy perf với quyền root |
| `perf list` | Liệt kê event |
| `sudo perf stat -e ... ./cpu_test` | Đo các event chỉ định |

---

## 16. Checklist

- [x] Tạo `cpu_test.c`
- [x] Compile thành công
- [x] Chạy chương trình thành công
- [x] Kiểm tra kết quả `sum`
- [x] Hiểu lỗi `perf_event_paranoid = 4`
- [x] Chạy `perf stat` bằng `sudo`
- [x] Đo `cycles`
- [x] Đo `instructions`
- [x] Đọc IPC
- [x] Đo `branches`
- [x] Đo `branch-misses`
- [x] Đo `cache-references`
- [x] Đo `cache-misses`
- [x] Đo `context-switches`
- [x] Đo `page-faults`
- [x] Hiểu multiplexing của `perf`

---

## 17. Kết luận

Bài 3 đã sử dụng thành công `perf stat` để đo CPU event của một chương trình user-space.

Kết quả chính:

```text
cycles            = 1.497.860.794
instructions      = 1.204.905.527
IPC               = 0,80
branches          = 200.863.914
branch-misses     = 17.906
cache-references  = 64.864
cache-misses      = 8.184
cache miss rate   = 12,62%
context-switches  = 0
page-faults       = 58
time elapsed      = 0,593689445 s
```

Qua bài này đã hoàn thành mục tiêu:

```text
Dùng perf stat để đo CPU Events
```
