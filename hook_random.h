//
// Created by sciver on 10/17/22.
//

#ifndef LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HOOK_RANDOM_H
#define LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HOOK_RANDOM_H
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/version.h>
#include <linux/namei.h>
extern asmlinkage ssize_t (*orig_random_read)(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos);
extern asmlinkage ssize_t (*orig_urandom_read)(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos);
asmlinkage ssize_t hook_random_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos);
asmlinkage ssize_t hook_urandom_read(struct file *file, char __user *buf, size_t nbytes, loff_t *ppos);
extern asmlinkage long (*orig_random)(const struct pt_regs *);
asmlinkage int hook_random(const struct pt_regs *regs);
#endif //LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HOOK_RANDOM_H
