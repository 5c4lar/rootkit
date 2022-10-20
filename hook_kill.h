//
// Created by sciver on 10/17/22.
//

#ifndef LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HOOK_KILL_H
#define LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HOOK_KILL_H
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
extern asmlinkage long (*orig_kill)(const struct pt_regs *);
asmlinkage int hook_kill(const struct pt_regs *regs);
#endif //LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HOOK_KILL_H
