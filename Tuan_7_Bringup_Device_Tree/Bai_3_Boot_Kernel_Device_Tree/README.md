# BÀI 3 — BOOT KERNEL VỚI DEVICE TREE ĐÃ CHỈNH SỬA

## 1. Mục tiêu bài thực hành

Bài 3 tiếp tục từ Bài 2, sử dụng file Device Tree Blob (`.dtb`) đã được chỉnh sửa và kiểm tra thành công để đưa vào quá trình boot thực tế trên Raspberry Pi 3 Model B+.

Mục tiêu:

- Copy file DTB mới từ Ubuntu xuống Raspberry Pi.
- Backup DTB gốc và `config.txt`.
- Đưa DTB mới vào `/boot/firmware`.
- Cấu hình Raspberry Pi dùng DTB mới.
- Hiểu vai trò của `config.txt`, `sync`, `reboot`.
- Boot Raspberry Pi bằng Device Tree mới.
- Kiểm tra kernel, model board và Device Tree runtime sau reboot.
- Xác nhận node `week7_device` và các property `compatible`, `status`.
- Chuẩn bị kết quả cho Bài 4.

---

## 2. Môi trường thực hiện

### Máy phát triển

```text
Hệ điều hành : Ubuntu Linux
User         : do
Hostname     : do-X99
```

Thư mục làm việc:

```text
~/Downloads/BSP_Training/Tuan_7_Bringup_Device_Tree/Bai_3_Boot_Kernel_Device_Tree
```

### Board thực tế

```text
Board : Raspberry Pi 3 Model B+
User  : ledo
IP    : 10.42.0.108
```

Kết nối:

```text
Ubuntu ↔ Ethernet ↔ Raspberry Pi
                 ↓
                SSH
```

---

## 3. File đầu vào từ Bài 2

File sử dụng:

```text
bcm2710-rpi-3-b-plus-week7.dtb
```

File này đã chứa node:

```dts
week7_device {
    compatible = "ledo,week7-device";
    status = "okay";
    device-name = "virtual-device";
    student-value = <0x4d2>;
};
```

Luồng:

```text
Bài 2
week7.dts
   ↓ dtc
week7.dtb
   ↓
Bài 3
Boot với DTB mới
```

---

## 4. Tạo thư mục Bài 3

```bash
cd ~/Downloads/BSP_Training/Tuan_7_Bringup_Device_Tree
mkdir Bai_3_Boot_Kernel_Device_Tree
cd Bai_3_Boot_Kernel_Device_Tree
```

Kiểm tra:

```bash
pwd
```

Kết quả mong đợi:

```text
/home/do/Downloads/BSP_Training/Tuan_7_Bringup_Device_Tree/Bai_3_Boot_Kernel_Device_Tree
```

---

## 5. Copy DTB từ Bài 2 sang Bài 3

```bash
cp ../Bai_2_Them_Device_Node/bcm2710-rpi-3-b-plus-week7.dtb .
```

Kiểm tra:

```bash
ls -lh
```

File cần có:

```text
bcm2710-rpi-3-b-plus-week7.dtb
```

Dấu `.` nghĩa là thư mục hiện tại.

---

## 6. Kiểm tra kết nối Raspberry Pi

```bash
ping -c 4 10.42.0.108
```

SSH:

```bash
ssh ledo@10.42.0.108
```

Mục đích:

- Xác nhận board đang online.
- Chuẩn bị backup và thay đổi cấu hình boot.

---

## 7. Backup DTB gốc

Lệnh:

```bash
sudo cp /boot/firmware/bcm2710-rpi-3-b-plus.dtb \
/boot/firmware/bcm2710-rpi-3-b-plus.dtb.backup
```

Ý nghĩa:

```text
DTB gốc
   ↓ copy
DTB backup
```

Backup là tạo bản sao dự phòng để có thể phục hồi nếu DTB mới gây lỗi boot.

Kiểm tra:

```bash
ls -lh /boot/firmware/bcm2710-rpi-3-b-plus.dtb*
```

---

## 8. Backup `config.txt`

```bash
sudo cp /boot/firmware/config.txt \
/boot/firmware/config.txt.week7-backup
```

Kiểm tra:

```bash
ls -lh /boot/firmware/config.txt*
```

Mục đích: nếu cấu hình mới gây lỗi, có thể khôi phục lại file cấu hình cũ.

---

## 9. Copy DTB mới xuống Raspberry Pi

Trên Ubuntu:

```bash
scp bcm2710-rpi-3-b-plus-week7.dtb \
ledo@10.42.0.108:/home/ledo/
```

Luồng:

```text
Ubuntu
week7.dtb
   ↓ SCP
Raspberry Pi
/home/ledo/
```

---

## 10. Copy DTB mới vào boot partition

SSH vào Raspberry Pi:

```bash
ssh ledo@10.42.0.108
```

Kiểm tra file:

```bash
ls -lh ~/bcm2710-rpi-3-b-plus-week7.dtb
```

Copy:

```bash
sudo cp ~/bcm2710-rpi-3-b-plus-week7.dtb \
/boot/firmware/
```

Kiểm tra:

```bash
ls -lh /boot/firmware/bcm2710-rpi-3-b-plus-week7.dtb
```

---

## 11. Kiểm tra DTB mới trước khi boot

```bash
dtc -I dtb -O dts \
-o /tmp/week7-check.dts \
/boot/firmware/bcm2710-rpi-3-b-plus-week7.dtb
```

Tìm node:

```bash
grep -A7 "week7_device" /tmp/week7-check.dts
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

Mục đích: xác nhận đúng DTB đã được đưa xuống board trước khi reboot.

---

## 12. Cấu hình sử dụng DTB mới

Mở:

```bash
sudo nano /boot/firmware/config.txt
```

Thêm:

```text
device_tree=bcm2710-rpi-3-b-plus-week7.dtb
```

Kiểm tra:

```bash
tail -15 /boot/firmware/config.txt
```

Luồng:

```text
config.txt
   ↓
device_tree=week7.dtb
   ↓
Raspberry Pi firmware
   ↓
Load DTB mới
   ↓
Linux Kernel
```

---

## 13. Vai trò của `config.txt`

`config.txt` là file cấu hình được Raspberry Pi firmware đọc trong giai đoạn đầu của quá trình boot.

Trong bài này, dòng:

```text
device_tree=bcm2710-rpi-3-b-plus-week7.dtb
```

dùng để chỉ định DTB mà firmware cần load.

---

## 14. Lệnh `sync`

Trước reboot:

```bash
sync
```

`sync` yêu cầu hệ thống ghi dữ liệu còn nằm trong cache xuống storage.

Luồng:

```text
RAM cache
   ↓ sync
Storage
```

Mục đích là giảm nguy cơ dữ liệu chưa được ghi hoàn toàn trước khi reboot.

---

## 15. Lệnh `reboot`

```bash
sudo reboot
```

`reboot` nghĩa là khởi động lại hệ thống.

Sau khi chạy, SSH bị đóng:

```text
Connection to 10.42.0.108 closed by remote host.
Connection to 10.42.0.108 closed.
```

Đây là hành vi bình thường.

---

## 16. Kiểm tra board sau reboot

Trên Ubuntu:

```bash
ping -c 4 10.42.0.108
```

Kết quả thực tế:

```text
4 packets transmitted, 2 received, 50% packet loss
```

Hai gói đầu có thể bị mất trong lúc Raspberry Pi đang boot. Hai gói sau nhận được phản hồi cho thấy board đã lên mạng.

SSH lại:

```bash
ssh ledo@10.42.0.108
```

Kết quả:

```text
Linux raspberrypi 6.18.50+rpt-rpi-v8 ... aarch64
```

Board đã boot thành công.

---

## 17. Kiểm tra `uptime`

```bash
uptime
```

Kết quả thực tế:

```text
14:50:11 up 0 min, 3 users, load average: 1.24, 0.39, 0.14
```

`up 0 min` xác nhận Raspberry Pi vừa reboot.

---

## 18. Kiểm tra kernel

```bash
uname -a
```

Kết quả thực tế:

```text
Linux raspberrypi 6.18.50+rpt-rpi-v8 #1 SMP PREEMPT Debian 1:6.18.50-1+rpt1 (2026-09-11) aarch64 GNU/Linux
```

Thông tin chính:

```text
Kernel       : 6.18.50+rpt-rpi-v8
Architecture : aarch64
```

---

## 19. Kiểm tra model board

```bash
cat /proc/device-tree/model
echo
```

Kết quả:

```text
Raspberry Pi 3 Model B Plus Rev 1.3
```

Điều này xác nhận kernel vẫn nhận đúng board sau khi boot bằng Device Tree mới.

---

## 20. Kiểm tra node `week7_device`

```bash
ls /proc/device-tree/week7_device
```

Kết quả thực tế:

```text
compatible
device-name
name
status
student-value
```

Đây là bằng chứng quan trọng nhất cho thấy Device Tree mới đã được kernel nhận.

Luồng:

```text
week7.dtb
   ↓
Firmware load
   ↓
Kernel parse
   ↓
/proc/device-tree/week7_device
```

---

## 21. Kiểm tra `compatible`

```bash
cat /proc/device-tree/week7_device/compatible
echo
```

Kết quả:

```text
ledo,week7-device
```

Giống với DTS:

```dts
compatible = "ledo,week7-device";
```

---

## 22. Kiểm tra `status`

```bash
cat /proc/device-tree/week7_device/status
echo
```

Kết quả:

```text
okay
```

Giống với DTS:

```dts
status = "okay";
```

Điều này xác nhận node được enable trong Device Tree runtime.

---

## 23. Mối quan hệ DTB và `/proc/device-tree`

```text
bcm2710-rpi-3-b-plus-week7.dtb
                ↓
Raspberry Pi firmware
                ↓
Linux Kernel
                ↓
Kernel parse DTB
                ↓
/proc/device-tree
                ↓
week7_device
```

DTB là dữ liệu đầu vào ở quá trình boot.

`/proc/device-tree` cho phép quan sát Device Tree mà kernel đang sử dụng tại runtime.

---

## 24. Tại sao phải reboot?

Sau khi thay DTB hoặc sửa `config.txt`, kernel đang chạy không tự động nạp lại toàn bộ Device Tree.

Cần reboot để firmware thực hiện lại:

```text
Firmware
   ↓
Đọc config.txt
   ↓
Load DTB mới
   ↓
Load Kernel
   ↓
Kernel parse DTB
```

---

## 25. Tại sao phải backup?

Device Tree ảnh hưởng trực tiếp tới việc kernel khởi tạo phần cứng.

Nếu DTB sai:

```text
DTB sai
  ↓
Kernel nhận mô tả phần cứng sai
  ↓
Thiết bị có thể không hoạt động
  ↓
Có thể mất network/SSH hoặc boot lỗi
```

Backup giúp có thể phục hồi hệ thống.

---

## 26. Cách phục hồi nếu DTB mới gây lỗi

Nếu Raspberry Pi không boot, tháo SD card và gắn vào Ubuntu.

Khôi phục `config.txt`:

```bash
sudo cp config.txt.week7-backup config.txt
```

Hoặc xóa dòng:

```text
device_tree=bcm2710-rpi-3-b-plus-week7.dtb
```

Nếu cần khôi phục DTB gốc:

```bash
sudo cp bcm2710-rpi-3-b-plus.dtb.backup \
bcm2710-rpi-3-b-plus.dtb
```

---

## 27. Tổng hợp câu lệnh đã sử dụng

```bash
cd ~/Downloads/BSP_Training/Tuan_7_Bringup_Device_Tree
mkdir Bai_3_Boot_Kernel_Device_Tree
cd Bai_3_Boot_Kernel_Device_Tree
```

```bash
cp ../Bai_2_Them_Device_Node/bcm2710-rpi-3-b-plus-week7.dtb .
```

```bash
ping -c 4 10.42.0.108
ssh ledo@10.42.0.108
```

```bash
sudo cp /boot/firmware/bcm2710-rpi-3-b-plus.dtb \
/boot/firmware/bcm2710-rpi-3-b-plus.dtb.backup
```

```bash
sudo cp /boot/firmware/config.txt \
/boot/firmware/config.txt.week7-backup
```

```bash
scp bcm2710-rpi-3-b-plus-week7.dtb \
ledo@10.42.0.108:/home/ledo/
```

```bash
sudo cp ~/bcm2710-rpi-3-b-plus-week7.dtb \
/boot/firmware/
```

```bash
sudo nano /boot/firmware/config.txt
```

Thêm:

```text
device_tree=bcm2710-rpi-3-b-plus-week7.dtb
```

```bash
sync
sudo reboot
```

Sau reboot:

```bash
ping -c 4 10.42.0.108
ssh ledo@10.42.0.108
uptime
uname -a
cat /proc/device-tree/model
echo
ls /proc/device-tree/week7_device
cat /proc/device-tree/week7_device/compatible
echo
cat /proc/device-tree/week7_device/status
echo
```

---

## 28. Những kiến thức đã học được

Sau Bài 3 đã học được:

1. Cách đưa DTB mới vào Raspberry Pi.
2. Cách backup file quan trọng trước khi chỉnh sửa.
3. Vai trò của `/boot/firmware`.
4. Vai trò của `config.txt`.
5. Vai trò của `device_tree=`.
6. Cách truyền DTB bằng SCP.
7. Ý nghĩa của `sync`.
8. Ý nghĩa của `reboot`.
9. Tại sao cần reboot sau khi thay Device Tree.
10. Cách kiểm tra board boot thành công.
11. Cách dùng `uptime`.
12. Cách kiểm tra kernel bằng `uname -a`.
13. Cách kiểm tra model bằng `/proc/device-tree/model`.
14. Cách kiểm tra node mới trong `/proc/device-tree`.
15. Cách đọc `compatible`.
16. Cách đọc `status`.
17. Mối quan hệ giữa DTB và Device Tree runtime.
18. Tầm quan trọng của backup.
19. Cách phục hồi nếu DTB mới gây lỗi boot.
20. Cách chứng minh kernel thực sự đã sử dụng Device Tree mới.

---

## 29. Kết quả đạt được

Bài 3 đã thực hiện thành công:

- DTB mới đã được copy xuống Raspberry Pi.
- DTB gốc đã được backup.
- `config.txt` đã được backup.
- DTB mới đã được đặt trong `/boot/firmware`.
- Raspberry Pi được cấu hình sử dụng DTB mới.
- `sync` được thực hiện trước reboot.
- Raspberry Pi reboot thành công.
- SSH kết nối lại thành công.
- Kernel hoạt động bình thường.
- Board vẫn được nhận đúng:
  ```text
  Raspberry Pi 3 Model B Plus Rev 1.3
  ```
- Node `week7_device` xuất hiện tại:
  ```text
  /proc/device-tree/week7_device
  ```
- `compatible`:
  ```text
  ledo,week7-device
  ```
- `status`:
  ```text
  okay
  ```

Như vậy Raspberry Pi đã boot thành công bằng Device Tree đã chỉnh sửa.

---

## 30. Luồng thực hiện toàn bài

```text
Bài 2
week7.dtb
   ↓
Copy sang Bài 3
   ↓
Backup DTB/config
   ↓
SCP xuống Pi
   ↓
/boot/firmware
   ↓
config.txt
   ↓
device_tree=week7.dtb
   ↓
sync
   ↓
reboot
   ↓
Firmware
   ↓
Linux Kernel
   ↓
/proc/device-tree/week7_device
   ↓
Xác nhận thành công
```

---

## 31. Kết luận

Bài 3 đã chuyển Device Tree từ mức file sang quá trình boot thực tế trên Raspberry Pi.

Kết quả quan trọng nhất:

```text
week7_device
```

đã xuất hiện tại:

```text
/proc/device-tree/week7_device
```

Điều này chứng minh firmware đã load DTB mới và Linux Kernel đã parse Device Tree đã chỉnh sửa.

Bài 3 là bước nối giữa:

```text
Bài 2: tạo DTB
        ↓
Bài 3: boot bằng DTB
        ↓
Bài 4: kiểm tra sâu Device Tree runtime
```

Bài tiếp theo sẽ đọc đầy đủ các property của node, kiểm tra `student-value` và dump Device Tree runtime để xác minh kết quả.
