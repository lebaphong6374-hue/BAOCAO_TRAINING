#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int count = 1;

module_param(count, int, 0644);
MODULE_PARM_DESC(count, "Number of hello messages");

static int __init param_init(void)
{
    int i;

    pr_info("param_module: count = %d\n", count);

    for (i = 0; i < count; i++)
        pr_info("Hello Kernel %d\n", i + 1);

    return 0;
}

static void __exit param_exit(void)
{
    pr_info("param_module: goodbye\n");
}

module_init(param_init);
module_exit(param_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nguyen Minh Tuan");
MODULE_DESCRIPTION("Kernel Module Parameter Example");
