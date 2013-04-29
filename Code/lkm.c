#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <linux/sys_stat.h>
#include <linux/seq_file.h>

int read_info( char *page, char **start, off_t off, int count, int *eof, void *data );

static void *my_seq_start(struct seq_file *s, loff_t *pos) {
	static unsigned long counter = 0;
	if (*pos == 0) {
		counter = 0;
		return &counter;
	} else {
		*pos = 0;
		return NULL;
	}
}

static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos) {
	unsigned long *tmp_v = (unsigned long *) v;
	(*tmp_v)++;
	(*pos)++;
	if ((*tmp_v) < 16) {
		return tmp_v;
	}
	return NULL;
}

static void my_seq_stop(struct seq_file *s, void *v) {
	// Do nothing
}

static int my_seq_show(struct seq_file *s, void *v) {
	char buf[2000];
	unsigned long *tmp_v = (unsigned long *) v;
	int i = (*tmp_v);
	cse812_fetch(buf, i);

	seq_printf(s, "%s\n", buf);
	return 0;
}

static struct seq_operations my_seq_ops = {
	.start = my_seq_start,
	.next  = my_seq_next,
	.stop  = my_seq_stop,
	.show  = my_seq_show
};

static int my_open(struct inode *inode, struct file *file) {
	return seq_open(file, &my_seq_ops);
}

static struct file_operations my_file_ops = {
	.owner   = THIS_MODULE,
	.open    = my_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release
};


int init_module(void) {
	struct proc_dir_entry *entry;

	entry = create_proc_entry( "sys_stat", 0, NULL );

	if (entry) {
		cse812_clear();
		entry->proc_fops = &my_file_ops;
		printk(KERN_INFO "sys_stat created.\n");
	}
	return 0;
}

void cleanup_module(void) {
	remove_proc_entry("sys_stat", NULL);
	printk(KERN_INFO "Bye.........\n");
}

int read_info(char *page, char **start, off_t off, int count, int *eof, void *data) {
	if (off > 0) {
		*eof = 1;
		return 0;
	}
	//cse812_fetch(page);
	return strlen(page);
}

MODULE_AUTHOR("Joseph Roth and Scott Klum");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("CSE 812 Semester Project.");
