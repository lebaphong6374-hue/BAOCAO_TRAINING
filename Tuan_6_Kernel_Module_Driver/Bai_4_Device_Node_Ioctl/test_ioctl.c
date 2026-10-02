#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_FILE "/dev/mychardev"

#define MY_MAGIC      'M'

#define IOCTL_CLEAR   _IO(MY_MAGIC, 1)

#define IOCTL_GET_LEN _IOR(MY_MAGIC, 2, int)


int main(void)
{
    int fd;

    int len;


    fd = open(DEVICE_FILE, O_RDWR);

    if (fd < 0) {

        perror("open");

        return 1;
    }


    printf("Device opened successfully\n");


    if (ioctl(fd,
              IOCTL_GET_LEN,
              &len) < 0) {

        perror("IOCTL_GET_LEN");

        close(fd);

        return 1;
    }


    printf("Buffer length = %d bytes\n",
           len);


    printf("Clearing buffer...\n");


    if (ioctl(fd,
              IOCTL_CLEAR) < 0) {

        perror("IOCTL_CLEAR");

        close(fd);

        return 1;
    }


    if (ioctl(fd,
              IOCTL_GET_LEN,
              &len) < 0) {

        perror("IOCTL_GET_LEN");

        close(fd);

        return 1;
    }


    printf("Buffer length after clear = %d bytes\n",
           len);


    close(fd);

    return 0;
}
