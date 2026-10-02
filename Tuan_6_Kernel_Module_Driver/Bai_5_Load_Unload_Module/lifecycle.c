#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init lifecycle_init(void)
{
    pr_info("lifecycle: module loaded into kernel\n");

    return 0;
}

static void __exit lifecycle_exit(void)
{
    pr_info("lifecycle: module removed from kernel\n");
}

module_init(lifecycle_init);
module_exit(lifecycle_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Le Ba Do");
MODULE_DESCRIPTION("Kernel Module Load Unload Example");
MODULE_VERSION("1.0");
