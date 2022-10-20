//
// Created by sciver on 10/17/22.
//

#ifndef LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HIDE_MODULE_H
#define LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HIDE_MODULE_H
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
void hideme(void);
void showme(void);
extern short hidden;
extern struct list_head *prev_module;
//extern asmlinkage long (*orig_kill_hide_module)(const struct pt_regs *);
//asmlinkage int hook_kill_hide_module(const struct pt_regs *regs);
#endif //LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HIDE_MODULE_H
