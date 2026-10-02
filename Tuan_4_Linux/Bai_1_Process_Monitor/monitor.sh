#!/bin/bash

# Kiem tra nguoi dung co nhap ten process hay khong
if [ $# -ne 1 ]; then
    echo "Cach dung: $0 <ten_process>"
    echo "Vi du: $0 bash"
    exit 1
fi

PROCESS_NAME=$1
LOG_FILE="process_monitor.log"

echo "=================================="
echo "Bat dau giam sat process: $PROCESS_NAME"
echo "Nhan Ctrl+C de dung"
echo "=================================="

while true
do
    PID=$(pgrep -x "$PROCESS_NAME" | head -n 1)

    if [ -n "$PID" ]; then
        MESSAGE="$(date '+%Y-%m-%d %H:%M:%S') - Process $PROCESS_NAME dang chay - PID=$PID"
    else
        MESSAGE="$(date '+%Y-%m-%d %H:%M:%S') - Process $PROCESS_NAME KHONG chay"
    fi

    echo "$MESSAGE"

    echo "$MESSAGE" >> "$LOG_FILE"

    sleep 2
done