//
// Created by sciver on 10/20/22.
//

#ifndef LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HIDE_DRIVER_H
#define LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HIDE_DRIVER_H
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
void hidemodule(char *name1);
void showmodule(char *name1);
#endif //LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HIDE_DRIVER_H
