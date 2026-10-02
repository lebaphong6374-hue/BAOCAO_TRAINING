# BÀI 4 — KIỂM TRA DEVICE TREE RUNTIME TRÊN RASPBERRY PI 3 MODEL B+

## 1. Mục tiêu bài thực hành

Bài 4 là bước cuối của chuỗi bài thực hành Device Tree tuần 7.

Sau khi:

- Bài 1: đọc và phân tích Device Tree gốc.
- Bài 2: thêm node `week7_device`.
- Bài 3: boot Raspberry Pi bằng DTB đã chỉnh sửa.

Bài 4 tập trung kiểm tra **Device Tree mà Linux Kernel đang thực sự sử dụng tại runtime**.

Mục tiêu:

- Hiểu `/proc/device-tree` là gì.
- Phân biệt DTS, DTB và Runtime Device Tree.
- Kiểm tra node `week7_device` sau khi kernel boot.
- Đọc các property `compatible`, `status`, `device-name`, `student-value`.
- Hiểu vì sao property dạng chuỗi có thể đọc bằng `cat`.
- Hiểu vì sao property dạng số nên đọc bằng `hexdump`.
- Kiểm tra giá trị `1234` ở dạng byte `00 00 04 d2`.
- Dump toàn bộ Device Tree runtime bằng `dtc -I fs`.
- Lưu kết quả thực hành thành file.
- Copy kết quả từ Raspberry Pi về Ubuntu.
- Hoàn thiện luồng end-to-end:
  `DTS → DTB → Boot → Runtime Device Tree`.

---

## 2. Môi trường thực hiện

### Máy phát triển

```text
Hệ điều hành : Ubuntu Linux
User         : do
Hostname     : do-X99
```

Workspace:

```text
~/Downloads/BSP_Training/Tuan_7_Bringup_Device_Tree
```

Thư mục Bài 4:

```text
~/Downloads/BSP_Training/Tuan_7_Bringup_Device_Tree/Bai_4_Kiem_Tra_Device_Tree
```

### Board thực tế

```text
Board : Raspberry Pi 3 Model B+
User  : ledo
IP    : 10.42.0.108
```

Kết nối:

```text
Ubuntu
   ↓
Ethernet
   ↓
Raspberry Pi
   ↓
SSH
```

---

## 3. Điều kiện trước khi làm Bài 4

Ở Bài 2 đã thêm node:

```dts
week7_device {
    compatible = "ledo,week7-device";
    status = "okay";
    device-name = "virtual-device";
    student-value = <1234>;
};
```

Sau khi compile và decompile, giá trị số có thể hiển thị thành:

```dts
student-value = <0x4d2>;
```

Vì:

```text
1234 decimal = 0x4D2 hexadecimal
```

Ở Bài 3 đã xác nhận node xuất hiện tại:

```text
/proc/device-tree/week7_device
```

Do đó Bài 4 sẽ kiểm tra sâu hơn node này.

---

## 4. Tạo thư mục Bài 4

Trên Ubuntu:

```bash
cd ~/Downloads/BSP_Training/Tuan_7_Bringup_Device_Tree
mkdir -p Bai_4_Kiem_Tra_Device_Tree
cd Bai_4_Kiem_Tra_Device_Tree
```

Kiểm tra:

```bash
pwd
```

Kết quả mong đợi:

```text
/home/do/Downloads/BSP_Training/Tuan_7_Bringup_Device_Tree/Bai_4_Kiem_Tra_Device_Tree
```

---

## 5. SSH vào Raspberry Pi

```bash
ssh ledo@10.42.0.108
```

Sau khi đăng nhập:

```text
ledo@raspberrypi:~ $
```

---

## 6. Kiểm tra Device Tree runtime

Linux expose Device Tree mà kernel đang sử dụng tại:

```text
/proc/device-tree
```

Kiểm tra:

```bash
ls /proc/device-tree
```

Có thể thấy các node như:

```text
aliases
chosen
cpus
memory
model
soc
week7_device
...
```

---

## 7. Ý nghĩa của `/proc/device-tree`

`/proc/device-tree` là representation của Device Tree mà kernel đã parse khi boot.

Luồng:

```text
DTS
 ↓
dtc
 ↓
DTB
 ↓
Raspberry Pi Firmware
 ↓
Linux Kernel
 ↓
Kernel parse DTB
 ↓
/proc/device-tree
```

Do đó:

```text
/boot/firmware/*.dtb
```

là file Device Tree Blob nằm trên storage.

Còn:

```text
/proc/device-tree
```

là Device Tree đang được kernel sử dụng tại runtime.

---

## 8. Kiểm tra model board

```bash
cat /proc/device-tree/model
echo
```

Kết quả mong đợi:

```text
Raspberry Pi 3 Model B Plus Rev 1.3
```

---

## 9. Kiểm tra node `week7_device`

```bash
ls /proc/device-tree/week7_device
```

Kết quả mong đợi:

```text
compatible
device-name
name
status
student-value
```

Điều này chứng minh node thêm ở Bài 2 đã tồn tại trong Device Tree runtime.

---

## 10. Kiểm tra property `compatible`

```bash
cat /proc/device-tree/week7_device/compatible
echo
```

Kết quả:

```text
ledo,week7-device
```

Trong DTS:

```dts
compatible = "ledo,week7-device";
```

`compatible` là property quan trọng giúp kernel match Device Tree node với driver.

Ví dụ:

```c
static const struct of_device_id week7_of_match[] = {
    { .compatible = "ledo,week7-device" },
    {}
};
```

Luồng:

```text
Device Tree Node
compatible = "ledo,week7-device"
            ↓
Kernel matching
            ↓
Platform Driver
            ↓
probe()
```

---

## 11. Kiểm tra property `status`

```bash
cat /proc/device-tree/week7_device/status
echo
```

Kết quả:

```text
okay
```

Trong DTS:

```dts
status = "okay";
```

Thông thường:

- `okay`: thiết bị được enable.
- `disabled`: thiết bị bị disable.

---

## 12. Kiểm tra `device-name`

```bash
cat /proc/device-tree/week7_device/device-name
echo
```

Kết quả mong đợi:

```text
virtual-device
```

Trong DTS:

```dts
device-name = "virtual-device";
```

Đây là property dạng chuỗi tự định nghĩa.

---

## 13. Kiểm tra `name`

```bash
cat /proc/device-tree/week7_device/name
echo
```

Kết quả thường là:

```text
week7_device
```

Property `name` phản ánh tên node.

---

## 14. Kiểm tra `student-value`

Trong DTS:

```dts
student-value = <1234>;
```

Đây là property dạng số/cell nên không nên chỉ đọc bằng `cat`.

Sử dụng:

```bash
hexdump -C /proc/device-tree/week7_device/student-value
```

Kết quả mong đợi:

```text
00000000  00 00 04 d2                                       |....|
00000004
```

---

## 15. Giải thích `00 00 04 d2`

Ta có:

```text
1234₁₀ = 0x04D2₁₆
```

Một cell Device Tree thông thường có kích thước:

```text
32 bit = 4 byte
```

Do đó:

```text
0x000004D2
```

được biểu diễn thành:

```text
00 00 04 d2
```

Có thể hình dung:

```text
32 bit
┌────┬────┬────┬────┐
│ 00 │ 00 │ 04 │ D2 │
└────┴────┴────┴────┘
```

Kiểm tra lại:

```bash
printf "%d
" 0x4d2
```

Kết quả:

```text
1234
```

---

## 16. Vì sao `cat` phù hợp với string nhưng không phù hợp với integer?

Ví dụ chuỗi:

```dts
compatible = "ledo,week7-device";
```

có thể đọc bằng:

```bash
cat /proc/device-tree/week7_device/compatible
```

Trong khi:

```dts
student-value = <1234>;
```

là dữ liệu nhị phân dạng cell, nên dùng:

```bash
hexdump -C /proc/device-tree/week7_device/student-value
```

Kiến thức rút ra:

```text
String property  → cat
Binary/cell      → hexdump
```

---

## 17. Dump toàn bộ Runtime Device Tree

```bash
dtc -I fs -O dts -o ~/week7_result/running-device-tree.dts /proc/device-tree
```

Ý nghĩa:

```text
-I fs
```

input là filesystem representation của Device Tree.

```text
-O dts
```

output là Device Tree Source.

Luồng:

```text
/proc/device-tree
        ↓
       dtc
        ↓
running-device-tree.dts
```

---

## 18. So sánh `dtc -I dtb` và `dtc -I fs`

Ở Bài 1:

```bash
dtc -I dtb -O dts
```

dùng cho:

```text
DTB → DTS
```

Ở Bài 4:

```bash
dtc -I fs -O dts
```

dùng cho:

```text
Runtime Device Tree filesystem → DTS
```

Đây là sự khác biệt quan trọng.

---

## 19. Kiểm tra file dump

```bash
ls -lh ~/week7_result
```

Cần thấy:

```text
running-device-tree.dts
```

---

## 20. Tìm node trong Runtime DTS

```bash
grep -n -A7 "week7_device" ~/week7_result/running-device-tree.dts
```

Kết quả mong đợi:

```dts
week7_device {
    compatible = "ledo,week7-device";
    status = "okay";
    device-name = "virtual-device";
    student-value = <0x4d2>;
};
```

---

## 21. So sánh Device Tree qua ba giai đoạn

### Giai đoạn 1 — DTS

```text
bcm2710-rpi-3-b-plus-week7.dts
```

### Giai đoạn 2 — DTB

```text
bcm2710-rpi-3-b-plus-week7.dtb
```

### Giai đoạn 3 — Runtime

```text
/proc/device-tree/week7_device
```

Luồng:

```text
DTS
 ↓
DTB
 ↓
Boot
 ↓
Runtime Device Tree
```

---

## 22. Copy kết quả về Ubuntu

Thoát Raspberry Pi:

```bash
exit
```

Trên Ubuntu:

```bash
cd ~/Downloads/BSP_Training/Tuan_7_Bringup_Device_Tree/Bai_4_Kiem_Tra_Device_Tree
```

Copy Runtime DTS:

```bash
scp ledo@10.42.0.108:/home/ledo/week7_result/running-device-tree.dts .
```

Copy file kết quả:

```bash
scp ledo@10.42.0.108:/home/ledo/week7_result/result.txt .
```

---

## 23. Cấu trúc thư mục Bài 4

### `README.md`

Mô tả toàn bộ quá trình, kiến thức và câu lệnh.

### `running-device-tree.dts`

Device Tree runtime được dump từ `/proc/device-tree`.

### `result.txt`

Lưu kết quả kiểm tra node và property.

---

## 24. Kiểm tra lại trên Ubuntu

```bash
grep -A7 "week7_device" running-device-tree.dts
```

Kiểm tra file kết quả:

```bash
cat result.txt
```

---

## 25. Tổng hợp toàn bộ câu lệnh Bài 4

```bash
ssh ledo@10.42.0.108
```

```bash
ls /proc/device-tree
```

```bash
cat /proc/device-tree/model
echo
```

```bash
ls /proc/device-tree/week7_device
```

```bash
cat /proc/device-tree/week7_device/compatible
echo
```

```bash
cat /proc/device-tree/week7_device/status
echo
```

```bash
cat /proc/device-tree/week7_device/device-name
echo
```

```bash
cat /proc/device-tree/week7_device/name
echo
```

```bash
hexdump -C /proc/device-tree/week7_device/student-value
```

```bash
printf "%d
" 0x4d2
```

```bash
mkdir -p ~/week7_result
```

```bash
dtc -I fs -O dts -o ~/week7_result/running-device-tree.dts /proc/device-tree
```

```bash
grep -n -A7 "week7_device" ~/week7_result/running-device-tree.dts
```

```bash
scp ledo@10.42.0.108:/home/ledo/week7_result/running-device-tree.dts .
```

```bash
scp ledo@10.42.0.108:/home/ledo/week7_result/result.txt .
```

---

## 26. Những kiến thức đã học được

Sau Bài 4 đã học được:

1. `/proc/device-tree` là Runtime Device Tree.
2. Device Tree runtime khác với file DTB trên storage.
3. Kernel parse DTB trong quá trình boot.
4. Có thể kiểm tra node bằng filesystem.
5. Có thể đọc property dạng chuỗi bằng `cat`.
6. Có thể đọc property binary/cell bằng `hexdump`.
7. Một cell Device Tree thông thường có kích thước 32 bit.
8. `1234` decimal tương ứng `0x4D2`.
9. `0x000004D2` được lưu thành `00 00 04 d2`.
10. Có thể dump Runtime Device Tree bằng `dtc -I fs -O dts`.
11. Có thể verify node sau boot.
12. Hiểu mối quan hệ:
    ```text
    DTS → DTB → Firmware → Kernel → /proc/device-tree
    ```
13. Hiểu rằng node tồn tại trong DTS chưa đủ; cần xác nhận nó xuất hiện trong runtime.
14. Biết cách lưu kết quả vào `result.txt`.
15. Biết cách copy kết quả từ Pi về Ubuntu bằng SCP.

---

## 27. Kết quả cần đạt

Bài 4 hoàn thành khi:

- `/proc/device-tree/week7_device` tồn tại.
- Có:
  ```text
  compatible
  device-name
  name
  status
  student-value
  ```
- `compatible`:
  ```text
  ledo,week7-device
  ```
- `status`:
  ```text
  okay
  ```
- `device-name`:
  ```text
  virtual-device
  ```
- `student-value`:
  ```text
  00 00 04 d2
  ```
- `running-device-tree.dts` chứa node `week7_device`.
- `result.txt` lưu đầy đủ kết quả.

---

## 28. Luồng kiến thức toàn bộ tuần 7

```text
BÀI 1
Đọc DTB gốc
   ↓
Decompile DTB → DTS
   ↓
Phân tích Device Tree

BÀI 2
Copy DTS
   ↓
Thêm week7_device
   ↓
Compile DTS → DTB
   ↓
Verify DTB

BÀI 3
Copy DTB xuống Pi
   ↓
Cấu hình config.txt
   ↓
Reboot
   ↓
Kernel boot bằng DTB mới

BÀI 4
/proc/device-tree
   ↓
Kiểm tra week7_device
   ↓
Đọc property
   ↓
Dump Runtime DTS
   ↓
Xác nhận end-to-end
```

---

## 29. Kết luận

Bài 4 hoàn thiện quá trình nghiên cứu Device Tree trên Raspberry Pi 3 Model B+.

Điểm quan trọng nhất là kiểm tra Device Tree mà Linux Kernel đang sử dụng thực tế tại runtime, thay vì chỉ kiểm tra file DTS hoặc DTB trên storage.

Chuỗi hoàn chỉnh:

```text
DTS source
    ↓
Device Tree Compiler
    ↓
DTB binary
    ↓
Raspberry Pi Firmware
    ↓
Linux Kernel
    ↓
Runtime Device Tree
    ↓
/proc/device-tree/week7_device
```

