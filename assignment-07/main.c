#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lancelot Owczarczak <contact@lancelot.life>");
MODULE_DESCRIPTION("DebugFS test");

static ssize_t id_file_write(struct file *filp, const char *buffer,
			     size_t length, loff_t * offset);

static ssize_t id_file_read(struct file *filp, char *buffer,
			    size_t length, loff_t * offset);

static struct dentry *Dfortytwo;

static struct dentry *Did;
static struct file_operations id_fops = {
	.read = id_file_read,
	.write = id_file_write,
};

static struct dentry *Djiffies;
static struct file_operations jiffies_fops = {
	// TODO: add jiffies operations
};

static struct dentry *Dfoo;
static struct file_operations foo_fops = {
	// TODO: add foo operations
};

static int error_cleanup(void)
{
	printk(KERN_ERR "Failed to create virtual files");
	debugfs_remove_recursive(Dfortytwo);
	return -1;
}

// TODO: The error handling doesn't seems to work, find why and fix it

static int __init hello_init(void)
{
	Dfortytwo = debugfs_create_dir("fortytwo", NULL);

	if ((long)Dfortytwo == -ENODEV) {
		printk(KERN_ERR
		       "DebugFS isn't mounted, \"mount -t debugfs none /sys/kernel/debug/\" to mount it");
		return -1;
	}

	if (Dfortytwo == NULL) {
		printk(KERN_ERR "The directory fortytwo can't be created");
		return -1;
	}

	Did = debugfs_create_file("id", 0666, Dfortytwo, NULL, &id_fops);

	if (Did == NULL || (long)Did == -ENODEV)
		return error_cleanup();

	Djiffies =
	    debugfs_create_file("jiffies", 0444, Dfortytwo, NULL,
				&jiffies_fops);

	if (Djiffies == NULL || (long)Djiffies == -ENODEV)
		return error_cleanup();

	Dfoo = debugfs_create_file("foo", 0644, Dfortytwo, NULL, &foo_fops);

	if (Dfoo == NULL || (long)Dfoo == -ENODEV)
		return error_cleanup();

	printk(KERN_INFO "Starting DebugFS test module!\n");
	return 0;
}

static void __exit hello_cleanup(void)
{
	debugfs_remove_recursive(Dfortytwo);
	printk(KERN_INFO "Cleaning up DebugFS test module.\n");
}

static ssize_t id_file_read(struct file *filp, char *buffer,
			    size_t length, loff_t * offset)
{
	int retval = length < 8 ? length : 8;

	strncpy(buffer, "lowczarc", retval);

	return retval;
}

static ssize_t id_file_write(struct file *filp, const char *buffer,
			     size_t length, loff_t * offset)
{
	if (length != 8 || strncmp(buffer, "lowczarc", 8)) {
		return -EINVAL;
	}

	return 8;
}

module_init(hello_init);
module_exit(hello_cleanup);
