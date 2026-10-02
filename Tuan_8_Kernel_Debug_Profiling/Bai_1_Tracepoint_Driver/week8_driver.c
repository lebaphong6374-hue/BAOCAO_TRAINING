#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define CREATE_TRACE_POINTS
#include "week8_trace.h"

#define DEVICE_NAME "week8_driver"
#define CLASS_NAME  "week8_class"
#define BUFFER_SIZE 256

static dev_t dev_number;

static struct cdev week8_cdev;

static struct class *week8_class;

static struct device *week8_device;

static char kernel_buffer[BUFFER_SIZE];

static size_t data_size;


/* =========================================
 * OPEN
 * ========================================= */
static int week8_open(struct inode *inode,
                      struct file *file)
{
    pr_info("week8_driver: device opened\n");

    return 0;
}


/* =========================================
 * RELEASE
 * ========================================= */
static int week8_release(struct inode *inode,
                         struct file *file)
{
    pr_info("week8_driver: device closed\n");

    return 0;
}


/* =========================================
 * READ
 * Kernel -> User
 * ========================================= */
static ssize_t week8_read(struct file *file,
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

    pr_info("week8_driver: read %zu bytes\n",
            bytes_to_read);

    return bytes_to_read;
}


/* =========================================
 * WRITE
 * User -> Kernel
 * ========================================= */
static ssize_t week8_write(struct file *file,
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

    /*
     * Kích hoạt custom tracepoint.
     */
    trace_week8_write(bytes_to_write);

    pr_info("week8_driver: wrote %zu bytes\n",
            bytes_to_write);

    return bytes_to_write;
}


/* =========================================
 * FILE OPERATIONS
 * ========================================= */
static const struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = week8_open,
    .read    = week8_read,
    .write   = week8_write,
    .release = week8_release,
};


/* =========================================
 * MODULE INIT
 * ========================================= */
static int __init week8_init(void)
{
    int ret;

    pr_info("week8_driver: loading module\n");

    ret = alloc_chrdev_region(
            &dev_number,
            0,
            1,
            DEVICE_NAME);

    if (ret < 0)
        return ret;

    pr_info("week8_driver: major=%d minor=%d\n",
            MAJOR(dev_number),
            MINOR(dev_number));

    cdev_init(&week8_cdev, &fops);

    week8_cdev.owner = THIS_MODULE;

    ret = cdev_add(
            &week8_cdev,
            dev_number,
            1);

    if (ret < 0) {
        unregister_chrdev_region(dev_number, 1);
        return ret;
    }

    week8_class = class_create(CLASS_NAME);

    if (IS_ERR(week8_class)) {
        cdev_del(&week8_cdev);
        unregister_chrdev_region(dev_number, 1);

        return PTR_ERR(week8_class);
    }

    week8_device = device_create(
            week8_class,
            NULL,
            dev_number,
            NULL,
            DEVICE_NAME);

    if (IS_ERR(week8_device)) {
        class_destroy(week8_class);
        cdev_del(&week8_cdev);
        unregister_chrdev_region(dev_number, 1);

        return PTR_ERR(week8_device);
    }

    memset(kernel_buffer, 0, BUFFER_SIZE);

    data_size = 0;

    pr_info("week8_driver: module loaded successfully\n");

    return 0;
}


/* =========================================
 * MODULE EXIT
 * ========================================= */
static void __exit week8_exit(void)
{
    device_destroy(week8_class, dev_number);

    class_destroy(week8_class);

    cdev_del(&week8_cdev);

    unregister_chrdev_region(dev_number, 1);

    pr_info("week8_driver: module unloaded\n");
}


module_init(week8_init);
module_exit(week8_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nguyen Minh Tuan");
MODULE_DESCRIPTION("Week 8 Character Driver with Custom Tracepoint");
MODULE_VERSION("1.0");
