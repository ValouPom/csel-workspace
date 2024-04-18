#include <linux/module.h>	// needed by all modules
#include <linux/init.h>		// needed for macros
#include <linux/kernel.h>	// needed for debugging

#include <linux/io.h>		/* needed for mmio handling */
#include <linux/interrupt.h>	/* needed for interrupt handling */
#include <linux/gpio.h>			/* needed for i/o handling */

#define GPIO_B1 0
#define GPIO_B2 2
#define GPIO_B3 3

static char* btn_1="gpio_a.0-k1";
static char* btn_2="gpio_a.2-k2";
static char* btn_3="gpio_a.3-k3";

irqreturn_t getPressedBtn(int irq, void *dev_id)
{
	pr_info("Btn %s has been trigger\n", (char*)dev_id);
    return IRQ_HANDLED;
}

static int __init skeleton_init(void)
{
	pr_info ("Linux module 02 skeleton loaded\n");
	int status = gpio_request(GPIO_B1, "GPIO_B1");
	if (status != 0) {
		pr_err("GPIO error");
	}
	status = request_irq(gpio_to_irq(GPIO_B1), getPressedBtn, IRQF_TRIGGER_RISING | IRQF_SHARED, btn_1, btn_1);
	if (status != 0) {
		pr_err("GPIO error");
	}

	status = gpio_request(GPIO_B2, "GPIO_B2");
	if (status != 0) {
		pr_err("GPIO error");
	}
	status = request_irq(gpio_to_irq(GPIO_B2), getPressedBtn, IRQF_TRIGGER_RISING | IRQF_SHARED, btn_2, btn_2);
	if (status != 0) {
		pr_err("GPIO error");
	}

	status = gpio_request(GPIO_B3, "GPIO_B3");
	if (status != 0) {
		pr_err("GPIO error");
	}
	status = request_irq(gpio_to_irq(GPIO_B3), getPressedBtn, IRQF_TRIGGER_RISING | IRQF_SHARED, btn_3, btn_3);
	if (status != 0) {
		pr_err("GPIO error");
	}
	return 0;
}

static void __exit skeleton_exit(void)
{
	gpio_free(GPIO_B1);
	free_irq (gpio_to_irq(GPIO_B1), btn_1);

	gpio_free(GPIO_B2);
	free_irq (gpio_to_irq(GPIO_B2), btn_2);

	gpio_free(GPIO_B3);
	free_irq (gpio_to_irq(GPIO_B3), btn_3);
	pr_info ("Linux module skeleton unloaded\n");

}

module_init (skeleton_init);
module_exit (skeleton_exit);

MODULE_AUTHOR ("SSY");
MODULE_DESCRIPTION ("Module skeleton");
MODULE_LICENSE ("GPL");
