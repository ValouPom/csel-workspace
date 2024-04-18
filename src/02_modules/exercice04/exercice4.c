#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/io.h>		/* needed for mmio handling */
#include <linux/moduleparam.h>  /* needed for module parameters */
#include <linux/slab.h>		/* needed for dynamic memory allocation */
#include <linux/list.h>		/* needed for linked list processing */
#include <linux/string.h>	/* needed for string handling */

static int number = 3;
module_param(number, int, 0);

static char* name = "test";
module_param(name, charp, 0664);

// definition of a list element with struct list_head as member
struct element {
    // some members
    struct list_head list;
    char name[20];
};

// definition of the global list
static LIST_HEAD (my_list);


static int __init skeleton_init(void)
{
    int i = 0;
	pr_info("number of instance: %d, name %s", number, name);

    for (i = 0; i < number; i++) {
        struct element* ele;
        ele = kzalloc(sizeof(*ele), GFP_KERNEL); 
        if (ele != NULL) {
            snprintf(ele->name, 20, "%s-%d", name, i);
            list_add_tail(&ele->list, &my_list); 
        }
    }

	pr_info ("Linux module 02 skeleton loaded\n");
	return 0;
}

static void __exit skeleton_exit(void)
{
    struct element* ele;
    struct element* next;
    list_for_each_entry(ele, &my_list, list) { 
        pr_info("Element:\t%s\n", ele->name);
    }

    list_for_each_entry_safe(ele, next, &my_list, list) {
        list_del(&ele->list);
        kfree(ele);
    }

	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("SSY");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
