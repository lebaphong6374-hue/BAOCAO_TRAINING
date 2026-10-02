#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DEVICE_NAME "mychardev"
#define CLASS_NAME  "mychar_class"
#define BUFFER_SIZE 256

static dev_t dev_number;

static struct cdev my_cdev;

static struct class *my_class;
static struct device *my_device;

static char kernel_buffer[BUFFER_SIZE];
static size_t data_size = 0;


/* Khi user open /dev/mychardev */
static int my_open(struct inode *inode,
                   struct file *file)
{
    pr_info("mychardev: device opened\n");

    return 0;
}


/* Khi user close device */
static int my_release(struct inode *inode,
                      struct file *file)
{
    pr_info("mychardev: device closed\n");

    return 0;
}


/* READ: Kernel -> User */
static ssize_t my_read(struct file *file,
                       char __user *user_buffer,
                       size_t len,
                       loff_t *offset)
{
    size_t bytes_to_read;

    if (*offset >= data_size)
        return 0;

    bytes_to_read = data_size - *offset;

    if (len < bytes_to_read)
        bytes_to_read = len;

    if (copy_to_user(user_buffer,
                     kernel_buffer + *offset,
                     bytes_to_read))
        return -EFAULT;

    *offset += bytes_to_read;

    pr_info("mychardev: read %zu bytes\n",
            bytes_to_read);

    return bytes_to_read;
}


/* WRITE: User -> Kernel */
static ssize_t my_write(struct file *file,
                        const char __user *user_buffer,
                        size_t len,
                        loff_t *offset)
{
    size_t bytes_to_write;

    if (len > BUFFER_SIZE)
        bytes_to_write = BUFFER_SIZE;
    else
        bytes_to_write = len;

    memset(kernel_buffer, 0, BUFFER_SIZE);

    if (copy_from_user(kernel_buffer,
                       user_buffer,
                       bytes_to_write))
        return -EFAULT;

    data_size = bytes_to_write;

    pr_info("mychardev: wrote %zu bytes\n",
            bytes_to_write);

    return bytes_to_write;
}


/* Bảng file_operations */
static const struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = my_open,
    .read    = my_read,
    .write   = my_write,
    .release = my_release,
};


/* Chạy khi insmod */
static int __init my_driver_init(void)
{
    int ret;

    pr_info("mychardev: loading module\n");

    /* Xin Kernel cấp Major/Minor */
    ret = alloc_chrdev_region(
                &dev_number,
                0,
                1,
                DEVICE_NAME);

    if (ret < 0) {
        pr_err("mychardev: cannot allocate device number\n");
        return ret;
    }

    pr_info("mychardev: major=%d minor=%d\n",
            MAJOR(dev_number),
            MINOR(dev_number));

    /* Nối character device với file_operations */
    cdev_init(&my_cdev, &fops);

    my_cdev.owner = THIS_MODULE;

    /* Đăng ký character device với Kernel */
    ret = cdev_add(
                &my_cdev,
                dev_number,
                1);

    if (ret < 0) {
        unregister_chrdev_region(dev_number, 1);
        return ret;
    }

    /* Tạo class */
    my_class = class_create(CLASS_NAME);

    if (IS_ERR(my_class)) {
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_number, 1);

        return PTR_ERR(my_class);
    }

    /* Tạo device */
    my_device = device_create(
                    my_class,
                    NULL,
                    dev_number,
                    NULL,
                    DEVICE_NAME);

    if (IS_ERR(my_device)) {
        class_destroy(my_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_number, 1);

        return PTR_ERR(my_device);
    }

    memset(kernel_buffer, 0, BUFFER_SIZE);

    data_size = 0;

    pr_info("mychardev: module loaded successfully\n");

    return 0;
}


/* Chạy khi rmmod */
static void __exit my_driver_exit(void)
{
    device_destroy(
        my_class,
        dev_number);

    class_destroy(
        my_class);

    cdev_del(
        &my_cdev);

    unregister_chrdev_region(
        dev_number,
        1);

    pr_info("mychardev: module unloaded\n");
}


module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nguyen Minh Tuan");
MODULE_DESCRIPTION("Simple Character Device Driver");
MODULE_VERSION("1.0");
