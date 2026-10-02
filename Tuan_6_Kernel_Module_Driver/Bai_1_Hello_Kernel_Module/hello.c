#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init hello_init(void)
{
    pr_info("Hello Kernel!\n");
    return 0;
}

static void __exit hello_exit(void)
{
    pr_info("Goodbye Kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Le Ba Do");
MODULE_DESCRIPTION("Hello World Kernel Module");
MODULE_VERSION("1.0");
