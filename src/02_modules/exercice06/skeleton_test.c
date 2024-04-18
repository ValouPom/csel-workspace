#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/io.h>		/* needed for mmio handling */
#include <linux/delay.h>
#include <linux/kthread.h>

static struct task_struct *thread1;

int run(void* params) {
	pr_info("thread started\n");
	while (!kthread_should_stop()) {
		ssleep(5);
		pr_info("Hello from thread\n");
    }
	return 0;
}

static int __init skeleton_init(void)
{
	pr_info ("Linux module 02 skeleton loaded\n");
	thread1 = kthread_run(run, NULL, "thread1");
	if (thread1 == NULL) {
		pr_err ("Can not run thread\n");
	}
	return 0;
}

static void __exit skeleton_exit(void)
{
	if (kthread_stop(thread1) != 0) {
		pr_err ("Connot stop thread\n");
	}
	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("SSY");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
