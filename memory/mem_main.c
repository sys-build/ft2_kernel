#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/mm_types.h>

MODULE_LICENSE("GPL");

extern char *get_buf(void);
extern int mem_proc_init(void);
extern int mem_proc_exit(void);

static long ft2_get_pid(char *buf) {
	char *tmp = NULL;
	long pid = -1;

	if (NULL == buf) {
		printk(KERN_ALERT "buf==NULL\n");
		goto get_pid_fail;
	}

	tmp = strstr(buf, "pid");
	if(NULL == tmp) {
		printk(KERN_ALERT "tmp==NULL, buf=%s\n", buf);
		goto get_pid_fail;
	}

	tmp += 4;
	while(*tmp == ' ') {
		tmp++;
	}

	buf = tmp;
	tmp = strchr(buf, ' ');
	if(tmp) {
		*tmp = '\0';
	}

	if(kstrtol(buf, 10, &pid) == 0) {
		*tmp = ' ';
		return pid;
	}

	printk(KERN_ALERT "kstrtol fail, tmp: %s\n", tmp);

get_pid_fail:
	printk(KERN_ALERT "get pid fail\n");
	return -1;
}

static unsigned long long ft2_get_malloc_addr(char *buf) {
	char *tmp = NULL;
	unsigned long long addr;

	if(NULL == buf) {
		printk(KERN_ALERT "buf==NULL\n");
		goto get_malloc_addr_fail;
	}

	tmp = strstr(buf, "malloc_addr:");
	if(NULL == tmp) {
		printk(KERN_ALERT "tmp==NULL, buf=%s\n", buf);
		goto get_malloc_addr_fail;
	}

	tmp += strlen("malloc_addr:");
	while(*tmp == ' ') {
		tmp++;
	}

	buf = tmp;
	tmp = strchr(buf, ' ');
	if(tmp) {
		*tmp = '\0';
	}
	else {
		tmp = strchr(buf, '\n');
		if(tmp) {
			*tmp = '\0';
		}
	}

	sscanf(buf, "%llx", &addr);
	return addr;

get_malloc_addr_fail:
	printk(KERN_ALERT "get_malloc_addr_fail\n");
	return 0;
}

void memory_handle(char *buf) {
	long pid = ft2_get_pid(buf);
	unsigned long long addr = ft2_get_malloc_addr(buf);
	struct task_struct *p = NULL;
	struct mm_struct *mm = NULL;

	if(pid < 0 || addr == 0) {
		goto memory_handle_fail;
	}

	// get pid & addr successful
	printk(KERN_ALERT "pid=%ld, addr=%p\n", pid, (void *)addr);

	// p = find_task_by_vpid(pid);
	p = pid_task(find_vpid(pid), PIDTYPE_PID);
	mm = p->mm;

	if(NULL == p) {
		printk(KERN_ALERT "task struct not found\n");
		goto memory_handle_fail;
	}

	if(NULL == mm) {
		printk(KERN_ALERT "mm_struct not found\n");
		goto memory_handle_fail;
	}

	printk(KERN_ALERT "process name: %s\n", p->comm);

	printk(KERN_ALERT "sizeof(unsigned long) = %ld\n", sizeof(unsigned long));
	printk(KERN_ALERT "mm->start_code=%p\n", (void *)mm->start_code);

	return;

memory_handle_fail:
	printk(KERN_ALERT "memory handle fail\n");
}

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
			memory_handle(buf);
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
