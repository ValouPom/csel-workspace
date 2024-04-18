#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/io.h>		/* needed for mmio handling */
#include <linux/interrupt.h>	/* needed for interrupt handling */
#include <linux/gpio.h>			/* needed for i/o handling */

#include <linux/device.h>          /* needed for sysfs handling */
#include <linux/platform_device.h> /* needed for sysfs handling */
#include <linux/miscdevice.h>

#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/atomic.h>
#include <linux/poll.h>  /* needed for polling handling */
#include <linux/sched.h> /* needed for scheduling constants */

#define GPIO_B1 0
#define GPIO_B2 2
#define GPIO_B3 3

static char* btn_1="gpio_a.0-k1";
static char* btn_2="gpio_a.2-k2";
static char* btn_3="gpio_a.3-k3";

static atomic_t nb_interrupts;
DECLARE_WAIT_QUEUE_HEAD(queue);

irqreturn_t getPressedBtn(int irq, void *dev_id)
{
	pr_info("Btn %s has been trigger\n", (char*)dev_id);

    atomic_inc(&nb_interrupts);

    wake_up_interruptible(&queue);

    return IRQ_HANDLED;
}

static ssize_t skeleton_read(struct file* f,
                             char __user* buf,
                             size_t sz,
                             loff_t* off)
{
    return 0;
}

static unsigned int skeleton_poll(struct file* f, poll_table* wait)
{
    unsigned mask = 0;
    poll_wait(f, &queue, wait);
    if (atomic_read(&nb_interrupts) != 0) {
        mask |= POLLIN | POLLRDNORM; /* read operation */

        atomic_dec(&nb_interrupts);
        pr_info("polling thread waked-up...\n");
    }
    return mask;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read  = skeleton_read,
    .poll  = skeleton_poll,
};

struct miscdevice misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .fops  = &fops,
    .name  = "mymodule",
    .mode  = 0777,
};

static int __init skeleton_init(void)
{
    int status;
    atomic_set(&nb_interrupts, 0);

    status = misc_register(&misc_device);
	if (status != 0) {
		pr_err("misc error");
	}

	pr_info ("Linux module 02 skeleton loaded\n");
	status = devm_request_irq(misc_device.this_device, gpio_to_irq(GPIO_B1), getPressedBtn, IRQF_TRIGGER_RISING | IRQF_SHARED, btn_1, btn_1);
	if (status != 0) {
		pr_err("GPIO error");
	}

	status = devm_request_irq(misc_device.this_device,gpio_to_irq(GPIO_B2), getPressedBtn, IRQF_TRIGGER_RISING | IRQF_SHARED, btn_2, btn_2);
	if (status != 0) {
		pr_err("GPIO error");
	}

	status = devm_request_irq(misc_device.this_device,gpio_to_irq(GPIO_B3), getPressedBtn, IRQF_TRIGGER_RISING | IRQF_SHARED, btn_3, btn_3);
	if (status != 0) {
		pr_err("GPIO error");
	}
	return 0;
}

static void __exit skeleton_exit(void)
{
	devm_free_irq(misc_device.this_device, gpio_to_irq(GPIO_B1), btn_1);

	devm_free_irq(misc_device.this_device,gpio_to_irq(GPIO_B2), btn_2);

	devm_free_irq(misc_device.this_device,gpio_to_irq(GPIO_B3), btn_3);

    misc_deregister(&misc_device);
	pr_info ("Linux module skeleton unloaded\n");

}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("SSY");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
