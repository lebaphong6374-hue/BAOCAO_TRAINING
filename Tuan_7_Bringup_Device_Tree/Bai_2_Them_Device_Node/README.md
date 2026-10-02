# BÀI 2 — THÊM DEVICE NODE VÀO DEVICE TREE

## 1. Mục tiêu bài thực hành

Bài 2 tiếp tục từ Bài 1, sử dụng file Device Tree Source (`.dts`) đã decompile từ Raspberry Pi 3 Model B+ để thực hiện chỉnh sửa Device Tree.

Mục tiêu chính của bài:

- Tạo một bản DTS mới từ file DTS gốc của Bài 1.
- Thêm một **device node giả lập** vào Device Tree.
- Hiểu cấu trúc của một Device Tree node.
- Hiểu vai trò của các property `compatible`, `status`, `device-name`, `student-value`.
- Hiểu quy tắc sắp xếp property và subnode trong DTS.
- Compile DTS thành DTB bằng `dtc`.
- Decompile DTB vừa tạo trở lại DTS.
- Kiểm tra xem node mới có thực sự tồn tại trong DTB hay không.
- Làm quen với `fdtdump`.
- Phân biệt warning với lỗi nghiêm trọng.
- Chuẩn bị file DTB mới cho Bài 3.

---

## 2. Môi trường thực hiện

### Máy phát triển

- Hệ điều hành: Ubuntu Linux
- User: `do`
- Hostname: `do-X99`

Thư mục làm việc:

```text
~/Downloads/BSP_Training/Tuan_7_Bringup_Device_Tree/Bai_2_Them_Device_Node
```

Công cụ sử dụng:

- `cp`
- `ls`
- `nano`
- `grep`
- `dtc`
- `fdtdump`

---

## 3. File đầu vào từ Bài 1

Bài 2 sử dụng file:

```text
../Bai_1_Doc_Sua_DTS/bcm2710-rpi-3-b-plus-original.dts
```

Luồng:

```text
Bài 1
bcm2710-rpi-3-b-plus-original.dts
            ↓
           copy
            ↓
Bài 2
bcm2710-rpi-3-b-plus-week7.dts
```

Việc tạo bản sao giúp giữ nguyên file DTS gốc của Bài 1.

---

## 4. Tạo thư mục Bài 2

```bash
cd ~/Downloads/BSP_Training/Tuan_7_Bringup_Device_Tree
mkdir Bai_2_Them_Device_Node
cd Bai_2_Them_Device_Node
```

---

## 5. Copy DTS gốc từ Bài 1

Lệnh:

```bash
cp ../Bai_1_Doc_Sua_DTS/bcm2710-rpi-3-b-plus-original.dts \
bcm2710-rpi-3-b-plus-week7.dts
```

Kiểm tra:

```bash
ls -lh
```

Kết quả thực tế cho thấy file mới đã được tạo:

```text
bcm2710-rpi-3-b-plus-week7.dts
```

---

## 6. Mở DTS để chỉnh sửa

```bash
nano bcm2710-rpi-3-b-plus-week7.dts
```

Mục đích:

- Mở Device Tree Source.
- Thêm một node thiết bị giả lập.
- Không sửa trực tiếp file gốc.

---

## 7. Device node đã thêm

Node được thêm:

```dts
week7_device {
    compatible = "ledo,week7-device";
    status = "okay";
    device-name = "virtual-device";
    student-value = <1234>;
};
```

---

## 8. Giải thích từng property

### `week7_device`

```dts
week7_device {
```

Đây là tên node thiết bị giả lập.

### `compatible`

```dts
compatible = "ledo,week7-device";
```

`compatible` dùng để định danh loại thiết bị và là cơ sở để Linux Kernel có thể match node Device Tree với driver phù hợp.

Ví dụ driver:

```c
static const struct of_device_id week7_of_match[] = {
    { .compatible = "ledo,week7-device" },
    {}
};
```

Luồng:

```text
Device Tree node
"ledo,week7-device"
        ↓
Linux Driver
"ledo,week7-device"
        ↓
probe()
```

### `status`

```dts
status = "okay";
```

`okay` thường có nghĩa node được enable.

Nếu là:

```dts
status = "disabled";
```

thì node thường được xem là bị disable.

### `device-name`

```dts
device-name = "virtual-device";
```

Đây là property dạng chuỗi được thêm để thực hành đọc property.

### `student-value`

```dts
student-value = <1234>;
```

Đây là property dạng số/cell.

Sau compile và decompile lại, giá trị có thể hiện thành:

```dts
student-value = <0x4d2>;
```

Vì:

```text
1234 decimal = 0x4D2 hexadecimal
```

Hai cách biểu diễn này cùng một giá trị.

---

## 9. Kiểm tra node vừa thêm

Lệnh:

```bash
grep -n -A7 "week7_device" bcm2710-rpi-3-b-plus-week7.dts
```

Ý nghĩa:

- `grep`: tìm chuỗi.
- `-n`: hiển thị số dòng.
- `-A7`: hiển thị thêm 7 dòng phía sau.

Kết quả thực tế:

```text
week7_device {
    compatible = "ledo,week7-device";
    status = "okay";
    device-name = "virtual-device";
    student-value = <1234>;
};
```

---

## 10. Quy tắc Property phải đứng trước Subnode

Trong cùng một node DTS, property phải nằm trước subnode.

Ví dụ đúng:

```dts
/ {
    compatible = "example,board";
    model = "Example Board";

    week7_device {
        compatible = "ledo,week7-device";
        status = "okay";
    };
};
```

Nếu đặt subnode trước rồi mới khai báo property của node cha, `dtc` có thể báo:

```text
Properties must precede subnodes
```

Đây là một lỗi quan trọng cần nhớ khi chỉnh sửa Device Tree.

---

## 11. Phân biệt cú pháp DTS và lệnh Linux

Trong quá trình thực hành đã có lần nhập:

```bash
.compatible = "ledo,week7-device"
```

trực tiếp vào terminal.

Bash báo:

```text
.compatible: command not found
```

Lý do:

```dts
compatible = "ledo,week7-device";
```

là cú pháp DTS, không phải lệnh shell.

DTS chỉ được viết trong file `.dts`.

---

## 12. Compile DTS thành DTB

Lệnh:

```bash
dtc -I dts -O dtb \
-o bcm2710-rpi-3-b-plus-week7.dtb \
bcm2710-rpi-3-b-plus-week7.dts
```

Ý nghĩa:

```text
-I dts
```

Input là DTS.

```text
-O dtb
```

Output là DTB.

```text
-o
```

Chỉ định tên file output.

Luồng:

```text
bcm2710-rpi-3-b-plus-week7.dts
                ↓
               dtc
                ↓
bcm2710-rpi-3-b-plus-week7.dtb
```

---

## 13. Warning khi compile

Trong quá trình compile xuất hiện nhiều warning như:

```text
unit_address_vs_reg
simple_bus_reg
avoid_unnecessary_addr_size
unique_unit_address
clocks_property
dmas_property
mboxes_property
phys_property
power_domains_property
thermal_sensors_property
gpios_property
interrupt_provider
```

Điểm quan trọng:

```text
Warning ≠ FATAL ERROR
```

Trong bài này, mặc dù có warning nhưng file DTB vẫn được tạo thành công.

Kiểm tra bằng:

```bash
ls -lh
```

Kết quả cho thấy có:

```text
bcm2710-rpi-3-b-plus-week7.dtb
bcm2710-rpi-3-b-plus-week7.dts
```

Vì vậy compile đã hoàn thành.

---

## 14. Decompile DTB để verify

Lệnh:

```bash
dtc -I dtb -O dts \
-o verify-week7.dts \
bcm2710-rpi-3-b-plus-week7.dtb
```

Mục đích:

- Kiểm tra DTB vừa tạo có thể đọc lại bằng `dtc`.
- Xác minh node `week7_device` thực sự đã được ghi vào DTB.

Luồng:

```text
week7.dts
   ↓
compile
   ↓
week7.dtb
   ↓
decompile
   ↓
verify-week7.dts
```

---

## 15. Kiểm tra node trong file verify

```bash
grep -A7 "week7_device" verify-week7.dts
```

Kết quả:

```dts
week7_device {
    compatible = "ledo,week7-device";
    status = "okay";
    device-name = "virtual-device";
    student-value = <0x4d2>;
};
```

Điều này xác nhận node mới đã nằm trong DTB.

---

## 16. Kiểm tra DTB bằng `fdtdump`

Lệnh:

```bash
fdtdump bcm2710-rpi-3-b-plus-week7.dtb | grep -A7 "week7_device"
```

Kết quả xác nhận:

```dts
week7_device {
    compatible = "ledo,week7-device";
    status = "okay";
    device-name = "virtual-device";
    student-value = <0x000004d2>;
};
```

`fdtdump` cũng cảnh báo rằng đây là công cụ debug mức thấp và khuyến nghị dùng:

```bash
dtc -I dtb -O dts
```

để decompile DTB thông thường.

---

## 17. So sánh trước và sau compile

Trước compile:

```dts
student-value = <1234>;
```

Sau compile/decompile:

```dts
student-value = <0x4d2>;
```

Hai giá trị tương đương:

```text
1234₁₀ = 0x4D2₁₆
```

---

## 18. Các file của Bài 2

Ý nghĩa:

### `README.md`

Mô tả toàn bộ bài thực hành.

### `bcm2710-rpi-3-b-plus-week7.dts`

DTS đã được thêm node `week7_device`.

### `bcm2710-rpi-3-b-plus-week7.dtb`

DTB mới được compile từ DTS đã chỉnh sửa.

Đây là file quan trọng sẽ dùng ở Bài 3.

### `verify-week7.dts`

DTS được decompile từ DTB mới để xác minh node.

---

## 19. Tổng hợp toàn bộ câu lệnh đã sử dụng

### Tạo thư mục

```bash
cd ~/Downloads/BSP_Training/Tuan_7_Bringup_Device_Tree
mkdir Bai_2_Them_Device_Node
cd Bai_2_Them_Device_Node
```

### Copy DTS

```bash
cp ../Bai_1_Doc_Sua_DTS/bcm2710-rpi-3-b-plus-original.dts \
bcm2710-rpi-3-b-plus-week7.dts
```

### Kiểm tra file

```bash
ls -lh
```

### Chỉnh sửa

```bash
nano bcm2710-rpi-3-b-plus-week7.dts
```

### Kiểm tra node

```bash
grep -n -A7 "week7_device" bcm2710-rpi-3-b-plus-week7.dts
```

### Compile DTS → DTB

```bash
dtc -I dts -O dtb \
-o bcm2710-rpi-3-b-plus-week7.dtb \
bcm2710-rpi-3-b-plus-week7.dts
```

### Kiểm tra DTB

```bash
ls -lh
```

### Decompile DTB → DTS

```bash
dtc -I dtb -O dts \
-o verify-week7.dts \
bcm2710-rpi-3-b-plus-week7.dtb
```

### Kiểm tra node trong file verify

```bash
grep -A7 "week7_device" verify-week7.dts
```

### Kiểm tra bằng `fdtdump`

```bash
fdtdump bcm2710-rpi-3-b-plus-week7.dtb | grep -A7 "week7_device"
```

---

## 20. Luồng thực hiện toàn bài

```text
DTS gốc từ Bài 1
        ↓
Copy thành week7.dts
        ↓
Thêm week7_device
        ↓
dtc compile
        ↓
week7.dtb
        ↓
dtc decompile
        ↓
verify-week7.dts
        ↓
grep / fdtdump
        ↓
Xác nhận node tồn tại
```

---

## 21. Những kiến thức đã học được

Sau Bài 2 đã học được:

1. Cách tạo bản sao DTS để chỉnh sửa an toàn.
2. Cách thêm một device node mới.
3. Cách khai báo property dạng chuỗi.
4. Cách khai báo property dạng số/cell.
5. Vai trò của `compatible`.
6. Vai trò của `status`.
7. Quy tắc property phải đứng trước subnode.
8. Cách compile DTS thành DTB.
9. Cách decompile DTB trở lại DTS.
10. Cách verify nội dung DTB.
11. Cách dùng `grep` để tìm node.
12. Cách dùng `fdtdump` để kiểm tra DTB.
13. Phân biệt warning và fatal error.
14. Phân biệt cú pháp DTS với Bash.
15. Hiểu giá trị decimal và hexadecimal có thể biểu diễn cùng một dữ liệu.
16. Hiểu mối quan hệ giữa Device Tree node và Linux driver thông qua `compatible`.

---

## 22. Kết quả đạt được

Bài thực hành đã thực hiện thành công:

- Tạo thư mục riêng cho Bài 2.
- Copy DTS gốc từ Bài 1.
- Tạo DTS mới để chỉnh sửa.
- Thêm thành công node `week7_device`.
- Thêm các property `compatible`, `status`, `device-name`, `student-value`.
- Compile DTS thành DTB thành công.
- File DTB mới đã được tạo.
- Decompile DTB ngược lại thành `verify-week7.dts`.
- Xác nhận node mới tồn tại sau compile/decompile.
- Kiểm tra lại bằng `fdtdump`.
- Xác nhận `1234` và `0x4d2` là cùng một giá trị.
- Chuẩn bị được DTB mới để sử dụng trong Bài 3.

---

## 23. Kết quả node cuối cùng

Node được xác nhận trong DTB:

```dts
week7_device {
    compatible = "ledo,week7-device";
    status = "okay";
    device-name = "virtual-device";
    student-value = <0x4d2>;
};
```

Điều này chứng minh quá trình:

```text
DTS
 ↓
Compile
 ↓
DTB
 ↓
Decompile
 ↓
DTS
```

đã giữ lại đầy đủ node mới.

---

## 24. Liên hệ với Linux Driver

Device Tree không phải driver.

Device Tree chỉ mô tả thiết bị.

```text
Device Tree
    ↓
Mô tả thiết bị
    ↓
compatible
    ↓
Linux Driver
    ↓
probe()
    ↓
Điều khiển phần cứng
```

Ở Bài 2 mới thực hiện phần mô tả thiết bị.

Kiến thức về `compatible` là nền tảng cho Platform Driver trong Embedded Linux.

---

## 25. Chuẩn bị cho Bài 3

File quan trọng nhất:

```text
bcm2710-rpi-3-b-plus-week7.dtb
```

Bài 3 sẽ dùng file này để:

```text
Copy DTB mới xuống Raspberry Pi
        ↓
Backup cấu hình cũ
        ↓
Cấu hình boot dùng DTB mới
        ↓
Reboot
        ↓
Kernel nhận Device Tree mới
```

## 26. Kết luận

Bài 2 là bước chuyển từ việc chỉ đọc Device Tree sang trực tiếp chỉnh sửa Device Tree.

Luồng chính:

```text
DTS gốc
   ↓
Tạo bản sao
   ↓
Thêm device node
   ↓
Thêm property
   ↓
Compile bằng dtc
   ↓
DTB mới
   ↓
Decompile để verify
   ↓
Kiểm tra bằng grep/fdtdump
```

Kết quả cuối cùng là file:

```text
bcm2710-rpi-3-b-plus-week7.dtb
```

đã chứa node:

```text
week7_device
```