CC = arm-none-eabi-gcc

CFLAGS = -mcpu=cortex-m3 -mthumb -Wall -Wextra

TARGET = hello.o

$(TARGET): hello.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean info

info: $(TARGET)
	arm-none-eabi-objdump -f $(TARGET)

clean:
	rm -f *.o