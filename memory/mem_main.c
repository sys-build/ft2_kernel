#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

extern char *get_buf(void);
extern int mem_proc_init(void);
extern int mem_proc_exit(void);

static int memory_init(void)
{
	printk(KERN_ALERT "memory study init\n");
	mem_proc_init();

	while(1) {
		char *buf = NULL;

		buf = get_buf();

		msleep(200);

		if(!memcmp(buf, "quit", 4)) {
			break;
		}

		if(memcmp(buf, "idle", 4)) {
			printk(KERN_ALERT "buffer: %s\n", buf);
			strcpy(buf, "idle");
		}
	}

	printk(KERN_ALERT "buffer: %s\n", get_buf());

	return 0;
}

static void memory_exit(void)
{
	printk(KERN_ALERT "memory study remove\n");
	mem_proc_exit();
}

module_init(memory_init);
module_exit(memory_exit);
