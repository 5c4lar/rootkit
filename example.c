//
// Created by sciver on 10/16/22.
//
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ucasqsl");
MODULE_DESCRIPTION("Basic Kernel Module");
MODULE_VERSION("0.01");

static int __init example_init(void)
{
    printk(KERN_INFO "Hello, world!\n");
    return 0;
}

static void __exit example_exit(void)
{
    printk(KERN_INFO "Goodbye, world!\n");
}

module_init(example_init);
module_exit(example_exit);
