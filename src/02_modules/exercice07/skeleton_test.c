#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/io.h>		/* needed for mmio handling */
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/atomic.h>

DECLARE_WAIT_QUEUE_HEAD(queue);
static atomic_t waitCondition;
static struct task_struct *thread1;
static struct task_struct *thread2;

int runThread1(void* params) {
	pr_info("thread 1 started\n");
	while (!kthread_should_stop()) {
		pr_info("Hello thread 1\n");
		atomic_inc(&waitCondition);
		ssleep(5);
		pr_info("Send msg 1\n");
		atomic_dec(&waitContidion);
		wake_up(&queue);
    }
	return 0;
}

int runThread2(void* params) {
	pr_info("thread 2 started\n");
	while (!kthread_should_stop()) {
		pr_info("Hello from thread 2\n");
		int status = wait_event_interruptible(queue, kthread_should_stop() || atomic_read(&waitCondition) == 0);
		if (status == -ERESTARTSYS) {
			pr_info("thread 2 interupt\n");
		}
		pr_info("wake up thread 2\n");
    }
	pr_info("End thread 2\n");
	return 0;
}

static int __init skeleton_init(void)
{

	pr_info ("Linux module 02 skeleton loaded\n");

	atomic_set(&waitCondition, 0);
	thread1 = kthread_run(runThread1, NULL, "thread1");
	if (thread1 == NULL) {
		pr_err ("Can not run thread\n");
	}

	thread2 = kthread_run(runThread2, NULL, "thread2");
	if (thread2 == NULL) {
		pr_err ("Can not run thread\n");
	}
	return 0;
}

static void __exit skeleton_exit(void)
{
	if (kthread_stop(thread1) != 0) {
		pr_err ("Connot stop thread\n");
	}
	if (kthread_stop(thread2) != 0) {
		pr_err ("Connot stop thread\n");
	}
	pr_info ("Linux module skeleton unloaded\n");
}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("SSY");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
