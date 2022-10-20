//
// Created by sciver on 10/16/22.
//
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/namei.h>
#include "reveal_ftrace.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ucasqsl");
MODULE_DESCRIPTION("mkdir syscall hook");
MODULE_VERSION("0.01");


/* init and exit functions where the hooking will happen later */
static int __init scan_init(void)
{
    printk(KERN_INFO "scan: start\n");
    reveal_ftrace();
    return 0;
}

static void __exit scan_exit(void)
{
    printk(KERN_INFO "scan: finished\n");
}

module_init(scan_init);
module_exit(scan_exit);
