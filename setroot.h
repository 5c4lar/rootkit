//
// Created by sciver on 10/17/22.
//

#ifndef LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_SETROOT_H
#define LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_SETROOT_H
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/version.h>
#include <linux/namei.h>
void set_root(void);
extern asmlinkage long (*orig_kill_setroot)(const struct pt_regs *);
extern asmlinkage int hook_kill_setroot(const struct pt_regs *regs);
#endif //LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_SETROOT_H
