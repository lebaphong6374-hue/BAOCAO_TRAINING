# BÀI 1 — ĐỌC VÀ PHÂN TÍCH DEVICE TREE TRÊN RASPBERRY PI 3 MODEL B+

## 1. Mục tiêu bài thực hành

Bài thực hành nhằm làm quen với **Device Tree** trong hệ thống Embedded Linux thông qua board **Raspberry Pi 3 Model B+**.

Sau bài này, người học có thể:

- Xác định đúng board phần cứng đang sử dụng.
- Xác định Device Tree Blob tương ứng với board.
- Hiểu Device Tree là gì và tại sao Linux Kernel cần Device Tree.
- Phân biệt được DTS và DTB.
- Biết cách lấy DTB thực tế từ Raspberry Pi.
- Biết cách sử dụng SSH để điều khiển Raspberry Pi từ máy Ubuntu.
- Biết cách sử dụng SCP để sao chép file giữa Ubuntu và Raspberry Pi.
- Biết cách sử dụng Device Tree Compiler (`dtc`).
- Biết cách decompile DTB thành DTS để đọc.
- Biết cách tìm và đọc các node quan trọng trong Device Tree.
- Hiểu sơ bộ cách Device Tree tham gia vào quá trình boot Linux.
- Chuẩn bị DTS gốc để sử dụng cho bài tiếp theo.

---

## 2. Môi trường thực hiện

### 2.1. Máy phát triển

Máy phát triển sử dụng:

- Hệ điều hành: Ubuntu Linux.
- Công cụ:
  - SSH
  - SCP
  - Device Tree Compiler (`dtc`)
  - Nano
  - Visual Studio Code
  - Các lệnh Linux cơ bản như `ls`, `cat`, `grep`, `uname`.

Máy Ubuntu được sử dụng để:

```text
Kết nối Raspberry Pi
        ↓
Lấy file DTB
        ↓
Chuyển DTB thành DTS
        ↓
Đọc và phân tích Device Tree
```

### 2.2. Board thực tế

Board sử dụng:

```text
Raspberry Pi 3 Model B+
```

Thông tin hệ thống:

```text
Hệ điều hành : Raspberry Pi OS 64-bit
User         : ledo
Kết nối      : Ethernet + SSH
```

---

## 3. Tổng quan về Device Tree

Device Tree là một cấu trúc dữ liệu mô tả phần cứng cho Linux Kernel.

Thay vì kernel phải hard-code toàn bộ thông tin phần cứng của từng board, các thông tin này được mô tả riêng trong Device Tree.

Device Tree có thể mô tả:

- CPU.
- Memory.
- GPIO.
- UART.
- I2C.
- SPI.
- USB.
- Interrupt Controller.
- Clock.
- Timer.
- DMA.
- Bus.
- Địa chỉ thanh ghi.
- Trạng thái thiết bị.
- Quan hệ giữa các thiết bị phần cứng.

Có thể hình dung:

```text
Phần cứng
   ↓
Device Tree
   ↓
Linux Kernel
   ↓
Driver
   ↓
Thiết bị hoạt động
```

Device Tree giúp tách phần mô tả phần cứng ra khỏi source code của Linux Kernel.

---

## 4. Phân biệt DTS và DTB

### 4.1. DTS

DTS là viết tắt của:

```text
Device Tree Source
```

Đây là file dạng text, con người có thể đọc và chỉnh sửa.

Ví dụ:

```dts
/ {
    model = "Raspberry Pi 3 Model B+";

    cpus {
        ...
    };

    soc {
        ...
    };
};
```

File DTS thường có phần mở rộng:

```text
.dts
```

### 4.2. DTB

DTB là viết tắt của:

```text
Device Tree Blob
```

DTB là dạng nhị phân được tạo ra từ DTS.

File DTB thường được firmware hoặc bootloader nạp vào bộ nhớ và chuyển cho Linux Kernel khi boot.

File có phần mở rộng:

```text
.dtb
```

### 4.3. Mối quan hệ DTS và DTB

```text
DTS
 │
 │ dtc
 ▼
DTB
 │
 ▼
Firmware / Bootloader
 │
 ▼
Linux Kernel
```

Có thể chuyển ngược từ DTB về DTS:

```text
DTB
 │
 │ dtc
 ▼
DTS
```

Việc decompile DTB sang DTS giúp đọc và phân tích Device Tree của board thực tế.

---

## 5. Device Tree Compiler

Công cụ được sử dụng là:

```text
dtc
```

Tên đầy đủ:

```text
Device Tree Compiler
```

Cài đặt trên Ubuntu:

```bash
sudo apt update
sudo apt install -y device-tree-compiler
```

Kiểm tra phiên bản:

```bash
dtc --version
```

`dtc` hỗ trợ:

```text
DTS → DTB
DTB → DTS
```

Ví dụ:

```bash
dtc -I dtb -O dts input.dtb -o output.dts
```

Ý nghĩa:

```text
-I dtb   : định dạng đầu vào là DTB
-O dts   : định dạng đầu ra là DTS
-o       : chỉ định file đầu ra
```

---

## 6. Kết nối Raspberry Pi bằng SSH

Từ máy Ubuntu, kết nối tới Raspberry Pi bằng:

```bash
ssh ledo@10.42.0.108
```

Sau khi đăng nhập thành công, terminal có dạng:

```text
ledo@raspberrypi:~ $
```

SSH giúp điều khiển Raspberry Pi từ xa mà không cần màn hình trực tiếp.

---

## 7. Kiểm tra model của board

Trên Raspberry Pi chạy:

```bash
cat /proc/device-tree/model
echo
```

Kết quả mong đợi:

```text
Raspberry Pi 3 Model B+
```

Lệnh trên đọc property `model` từ Device Tree mà kernel đang sử dụng.

Điều này xác nhận board thực tế là Raspberry Pi 3 Model B+.

---

## 8. Kiểm tra kiến trúc hệ thống

Chạy:

```bash
uname -m
```

Đối với hệ điều hành 64-bit, kết quả:

```text
aarch64
```

Có thể hiểu:

```text
aarch64
   ↓
ARM64
```

Kiểm tra kernel:

```bash
uname -r
```

Hoặc xem thông tin đầy đủ hơn:

```bash
uname -a
```

---

## 9. Kiểm tra Device Tree runtime

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
...
```

Có thể hình dung:

```text
DTB
 ↓
Kernel parse
 ↓
/proc/device-tree
```

Đây là phần rất quan trọng vì ở các bài tiếp theo, sau khi sửa Device Tree và reboot, có thể kiểm tra trực tiếp xem kernel đã nhận Device Tree mới hay chưa.

---

## 10. Xác định DTB của Raspberry Pi 3 Model B+

Kiểm tra boot partition:

```bash
ls /boot/firmware
```

Có thể thấy nhiều file DTB cho các model Raspberry Pi khác nhau, ví dụ:

```text
bcm2710-rpi-3-b.dtb
bcm2710-rpi-3-b-plus.dtb
bcm2711-rpi-4-b.dtb
bcm2712-rpi-5-b.dtb
...
```

Với Raspberry Pi 3 Model B+, file tương ứng là:

```text
bcm2710-rpi-3-b-plus.dtb
```

Kiểm tra:

```bash
ls -lh /boot/firmware/bcm2710-rpi-3-b-plus.dtb
```

Tên file có thể hiểu:

```text
bcm2710
```

là tên nền tảng/SoC family được sử dụng trong Device Tree.

```text
rpi-3-b-plus
```

cho biết đây là Device Tree dành cho Raspberry Pi 3 Model B+.

```text
.dtb
```

cho biết đây là Device Tree Blob.

---

## 11. Sao chép DTB từ Raspberry Pi về Ubuntu

Trên Ubuntu sử dụng:

```bash
scp ledo@10.42.0.108:/boot/firmware/bcm2710-rpi-3-b-plus.dtb \
bcm2710-rpi-3-b-plus-original.dtb
```

Trong đó:

```text
scp
```

là Secure Copy.

SCP sử dụng SSH để truyền file an toàn giữa hai máy.

Luồng thực hiện:

```text
Raspberry Pi
/boot/firmware/bcm2710-rpi-3-b-plus.dtb
            ↓
           SCP
            ↓
Ubuntu
bcm2710-rpi-3-b-plus-original.dtb
```

Việc thêm hậu tố `original` nhằm phân biệt file gốc với các file đã chỉnh sửa ở bài sau.

---

## 12. Kiểm tra file sau khi copy

Trên Ubuntu chạy:

```bash
ls -lh
```

Kiểm tra file:

```text
bcm2710-rpi-3-b-plus-original.dtb
```

Điều này xác nhận DTB đã được lấy từ board thực tế về máy phát triển.

---

## 13. Decompile DTB thành DTS

Thực hiện:

```bash
dtc -I dtb -O dts \
-o bcm2710-rpi-3-b-plus-original.dts \
bcm2710-rpi-3-b-plus-original.dtb
```

Luồng:

```text
bcm2710-rpi-3-b-plus-original.dtb
                 ↓
                dtc
                 ↓
bcm2710-rpi-3-b-plus-original.dts
```

Sau bước này có thể đọc Device Tree dưới dạng text.

Nếu `dtc` xuất hiện một số warning nhưng vẫn tạo được file DTS, có thể tiếp tục dùng file này để đọc và phân tích.

---

## 14. Tại sao cần decompile DTB?

DTB là binary nên không thuận tiện để đọc trực tiếp.

```text
DTB
→ binary
→ kernel sử dụng
→ con người khó đọc
```

Trong khi đó:

```text
DTS
→ text
→ con người đọc được
→ có thể chỉnh sửa
```

Vì vậy cần dùng:

```bash
dtc -I dtb -O dts
```

để decompile DTB thành DTS.

---

## 15. Mở file DTS

Có thể dùng Nano:

```bash
nano bcm2710-rpi-3-b-plus-original.dts
```

Hoặc Visual Studio Code:

```bash
code bcm2710-rpi-3-b-plus-original.dts
```

---

## 16. Cấu trúc tổng quát của Device Tree

Device Tree có cấu trúc dạng cây.

Ví dụ:

```text
/
├── aliases
├── chosen
├── cpus
├── memory
├── soc
│   ├── gpio
│   ├── serial
│   ├── i2c
│   ├── spi
│   ├── timer
│   └── interrupt-controller
└── ...
```

Node gốc được biểu diễn:

```dts
/ {
    ...
};
```

Tất cả các node khác nằm bên dưới node root.

---

## 17. Property và Node

Device Tree gồm hai thành phần cơ bản:

```text
Property
Node
```

Ví dụ property:

```dts
model = "Raspberry Pi 3 Model B+";
```

Trong đó:

```text
model
```

là tên property.

```text
"Raspberry Pi 3 Model B+"
```

là giá trị của property.

Ví dụ node:

```dts
cpus {
    ...
};
```

Một node có thể chứa:

```text
Property
Subnode
```

---

## 18. Quy tắc Property trước Subnode

Trong một node DTS, property phải nằm trước subnode.

Ví dụ đúng:

```dts
/ {
    compatible = "example";
    model = "example-board";

    device {
        status = "okay";
    };
};
```

Ví dụ sai:

```dts
/ {
    device {
        status = "okay";
    };

    compatible = "example";
};
```

Nếu đặt property sau subnode, `dtc` có thể báo:

```text
Properties must precede subnodes
```

Đây là một kiến thức rất quan trọng khi sửa DTS ở Bài 2.

---

## 19. Phân tích property `model`

Tìm:

```bash
grep -n "model =" bcm2710-rpi-3-b-plus-original.dts
```

Ví dụ:

```dts
model = "Raspberry Pi 3 Model B+";
```

Property này mô tả tên board.

---

## 20. Phân tích property `compatible`

Một property rất quan trọng là:

```dts
compatible = "...";
```

`compatible` được Linux sử dụng để xác định loại phần cứng và hỗ trợ matching giữa Device Tree node và driver.

Ví dụ Device Tree:

```dts
my_device {
    compatible = "ledo,my-device";
};
```

Driver Linux có thể khai báo:

```c
static const struct of_device_id my_of_match[] = {
    { .compatible = "ledo,my-device" },
    {}
};
```

Kernel dùng chuỗi `compatible` để match Device Tree với driver.

Luồng:

```text
DT node
"ledo,my-device"
       ↓
Platform Driver
"ledo,my-device"
       ↓
probe()
```

---

## 21. Node `cpus`

Tìm:

```bash
grep -n "cpus" bcm2710-rpi-3-b-plus-original.dts | head
```

Node `cpus` mô tả các CPU core của hệ thống.

Có thể chứa:

```text
CPU core
Enable method
Clock
Phandle
Các property liên quan CPU
```

---

## 22. Node `memory`

Node `memory` mô tả RAM của hệ thống.

Ví dụ dạng:

```dts
memory@0 {
    device_type = "memory";
    reg = <...>;
};
```

Kernel cần thông tin này để biết vùng bộ nhớ vật lý có thể sử dụng.

---

## 23. Node `soc`

Node:

```text
soc
```

là nơi mô tả các peripheral nằm trong SoC.

SoC là:

```text
System on Chip
```

Bên trong có thể chứa:

```text
GPIO
UART
SPI
I2C
Timer
Interrupt Controller
DMA
USB
```

---

## 24. Node GPIO

Tìm:

```bash
grep -n "gpio" bcm2710-rpi-3-b-plus-original.dts | head
```

GPIO là:

```text
General Purpose Input/Output
```

Linux sử dụng node GPIO để mô tả GPIO controller.

Driver GPIO sử dụng thông tin trong Device Tree để quản lý các chân GPIO.

---

## 25. Node UART

Tìm:

```bash
grep -n "serial" bcm2710-rpi-3-b-plus-original.dts | head
```

UART có thể được sử dụng cho:

```text
Serial console
Debug kernel
Giao tiếp MCU
Giao tiếp module ngoài
```

Các node `serial` mô tả bộ điều khiển UART trên SoC.

---

## 26. Node I2C

Tìm:

```bash
grep -n "i2c" bcm2710-rpi-3-b-plus-original.dts | head
```

I2C thường dùng để giao tiếp với:

```text
Sensor
EEPROM
RTC
Display controller
ADC/DAC
```

Device Tree có thể mô tả controller I2C và các thiết bị nằm trên bus đó.

---

## 27. Node SPI

Tìm:

```bash
grep -n "spi" bcm2710-rpi-3-b-plus-original.dts | head
```

SPI thường sử dụng các tín hiệu:

```text
SCLK
MOSI
MISO
CS
```

Device Tree có thể mô tả:

```text
SPI controller
Chip Select
Thiết bị được kết nối
Clock frequency
Compatible driver
```

---

## 28. Node `chosen`

Node:

```text
chosen
```

thường chứa các thông tin được firmware hoặc bootloader chuyển cho kernel.

Ví dụ:

```text
bootargs
stdout-path
initrd
```

Node này có liên quan trực tiếp đến quá trình boot.

---

## 29. Node `aliases`

Node:

```text
aliases
```

dùng để tạo tên ngắn cho các node Device Tree.

Ví dụ:

```text
serial0
i2c0
spi0
```

Điều này giúp hệ thống tham chiếu đến các node bằng tên dễ dùng hơn.

---

## 30. Property `status`

Một property phổ biến:

```dts
status = "okay";
```

Thông thường:

```text
okay
```

nghĩa là thiết bị được enable.

Còn:

```dts
status = "disabled";
```

nghĩa là thiết bị bị disable.

Đây là property sẽ được sử dụng ở Bài 2 khi thêm node thiết bị giả.

---

## 31. Property `reg`

Property:

```text
reg
```

thường dùng để mô tả:

```text
Địa chỉ base
Kích thước vùng thanh ghi
Địa chỉ thiết bị
```

Tùy loại bus mà `reg` có ý nghĩa khác nhau.

---

## 32. Property `interrupts`

Property:

```text
interrupts
```

dùng để mô tả interrupt của thiết bị.

Kernel sử dụng thông tin này để ánh xạ interrupt phần cứng cho driver.

---

## 33. Device Tree trong quá trình boot

Quá trình boot có thể hình dung:

```text
Power On
   ↓
Raspberry Pi Firmware
   ↓
Đọc config.txt
   ↓
Chọn kernel
   ↓
Chọn Device Tree
   ↓
Load kernel + DTB
   ↓
Linux Kernel khởi động
   ↓
Kernel parse DTB
   ↓
Khởi tạo subsystem
   ↓
Probe driver
   ↓
Userspace
```

Device Tree nằm ở giữa firmware và kernel, đóng vai trò cung cấp thông tin mô tả phần cứng.

---

## 34. Kernel sử dụng Device Tree như thế nào?

Kernel có thể đọc các thông tin như:

```text
compatible
reg
interrupts
clocks
gpios
status
DMA
bus information
```

Sau đó sử dụng các thông tin này để khởi tạo driver tương ứng.

---

## 35. Những lệnh Linux đã sử dụng

### SSH

```bash
ssh ledo@10.42.0.108
```

Dùng để điều khiển Raspberry Pi từ xa.

### SCP

```bash
scp source destination
```

Dùng để copy file thông qua SSH.

### ls

```bash
ls -lh
```

Dùng để kiểm tra file và dung lượng.

### cat

```bash
cat /proc/device-tree/model
```

Dùng để đọc nội dung file.

### grep

```bash
grep -n "gpio" file.dts
```

Dùng để tìm nội dung trong DTS.

### uname

```bash
uname -m
uname -r
uname -a
```

Dùng để kiểm tra:

```text
Architecture
Kernel version
System information
```

### dtc

```bash
dtc -I dtb -O dts ...
```

Dùng để chuyển đổi Device Tree.

---

### bcm2710-rpi-3-b-plus-original.dtb

Là Device Tree Blob gốc lấy trực tiếp từ Raspberry Pi.

File này được giữ nguyên để làm bản tham chiếu.

### bcm2710-rpi-3-b-plus-original.dts

Là Device Tree Source được decompile từ DTB gốc.

File này dùng để đọc, phân tích và làm cơ sở cho Bài 2.

---

## 37. Những kiến thức chính học được

Sau Bài 1, có thể tổng hợp các kiến thức chính:

### 37.1. Hiểu Device Tree

Device Tree mô tả phần cứng cho Linux Kernel.

### 37.2. Hiểu DTS

DTS là dạng source text của Device Tree.

### 37.3. Hiểu DTB

DTB là dạng binary được kernel sử dụng.

### 37.4. Biết sử dụng `dtc`

Có thể chuyển đổi:

```text
DTS ↔ DTB
```

### 37.5. Biết xác định DTB của board

Raspberry Pi 3 Model B+ sử dụng file:

```text
bcm2710-rpi-3-b-plus.dtb
```

### 37.6. Biết đọc Device Tree runtime

Device Tree kernel đang sử dụng có thể kiểm tra tại:

```text
/proc/device-tree
```

### 37.7. Hiểu cấu trúc node/property

Device Tree gồm:

```text
Node
Property
Subnode
```

### 37.8. Hiểu `compatible`

`compatible` là cơ sở quan trọng để kernel match thiết bị với driver.

### 37.9. Hiểu `status`

`status = "okay"` cho biết thiết bị được enable.

### 37.10. Hiểu Device Tree trong quá trình boot

```text
Firmware
   ↓
DTB
   ↓
Kernel
   ↓
Driver
```

### 37.11. Hiểu quy tắc property và subnode

Trong một node:

```text
Property
   ↓
Subnode
```

Property phải được khai báo trước subnode.

Nếu sai thứ tự có thể gặp lỗi:

```text
Properties must precede subnodes
```

---

## 38. Kết quả đạt được

Bài thực hành đã thực hiện được:

- Kết nối Raspberry Pi qua SSH.
- Xác định Raspberry Pi 3 Model B+.
- Xác định hệ thống đang chạy ARM64.
- Xác định DTB tương ứng với board.
- Copy DTB từ Raspberry Pi về Ubuntu bằng SCP.
- Cài đặt Device Tree Compiler.
- Decompile DTB thành DTS.
- Đọc cấu trúc Device Tree.
- Xác định các node quan trọng như CPU, Memory, GPIO, UART, I2C, SPI.
- Hiểu property và subnode.
- Hiểu vai trò của `compatible`.
- Hiểu vai trò của `status`.
- Hiểu mối quan hệ giữa Device Tree và Linux Kernel.
- Chuẩn bị được DTS gốc để sử dụng cho bài tiếp theo.

---

## 39. Luồng thực hiện toàn bài

```text
Raspberry Pi 3 Model B+
          ↓
Xác định model board
          ↓
Xác định DTB tương ứng
          ↓
bcm2710-rpi-3-b-plus.dtb
          ↓
SCP
          ↓
Ubuntu
          ↓
Device Tree Compiler
          ↓
DTB → DTS
          ↓
Đọc và phân tích Device Tree
          ↓
Chuẩn bị cho Bài 2
```

---

## 40. Kết luận

Bài 1 giúp xây dựng kiến thức nền tảng về Device Tree trong Embedded Linux.

Thông qua việc thao tác trực tiếp trên Raspberry Pi 3 Model B+, có thể quan sát Device Tree trên phần cứng thực tế thay vì chỉ học ở mức lý thuyết.

Quá trình thực hiện:

```text
Raspberry Pi
      ↓
Lấy DTB thật
      ↓
SCP
      ↓
Ubuntu
      ↓
dtc
      ↓
DTS
      ↓
Đọc và phân tích
```

Qua bài này có thể hiểu rằng Device Tree đóng vai trò cầu nối giữa mô tả phần cứng và Linux Kernel.


