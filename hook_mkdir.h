//
// Created by sciver on 10/20/22.
//

#ifndef LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HOOK_MKDIR_H
#define LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HOOK_MKDIR_H

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
extern asmlinkage long (*orig_mkdir)(const struct pt_regs *);
asmlinkage int hook_mkdir(const struct pt_regs *regs);
#endif //LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HOOK_MKDIR_H
