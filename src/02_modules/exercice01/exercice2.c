
#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/io.h>		/* needed for mmio handling */
#include <linux/moduleparam.h>  /* needed for module parameters */

static int number = 3;
module_param(number, int, 0);

static char* name = "test";
module_param(name, charp, 0664);

static int __init skeleton_init(void)
{
	pr_info ("Linux module 02 skeleton loaded\n");
	pr_info("number of instance: %d, name %s", number, name);
	return 0;
}

static void __exit skeleton_exit(void)
{
	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("SSY");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
