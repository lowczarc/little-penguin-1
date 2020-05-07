#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/slab.h>

#define LOGIN "lowczarc"
#define LOGIN_SIZE 8

static ssize_t id_file_write(struct file *filp, const char *buffer,
			     size_t length, loff_t * offset);

static ssize_t id_file_read(struct file *filp, char *buffer,
			    size_t length, loff_t * offset);

static ssize_t jiffies_file_read(struct file *filp, char *buffer,
				 size_t length, loff_t * offset);

static ssize_t foo_file_read(struct file *filp, char *buffer,
			     size_t length, loff_t * offset);

static ssize_t foo_file_write(struct file *filp, const char *buffer,
			      size_t length, loff_t * offset);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lancelot Owczarczak <contact@lancelot.life>");
MODULE_DESCRIPTION("DebugFS test");

static struct dentry *Dfortytwo;

static struct dentry *Did;
static struct file_operations id_fops = {
	.read = id_file_read,
	.write = id_file_write,
};

static struct dentry *Djiffies;
static struct file_operations jiffies_fops = {
	.read = jiffies_file_read,
};

static struct dentry *Dfoo;
static struct file_operations foo_fops = {
	.write = foo_file_write,
	.read = foo_file_read,
};

static char *Foo_stored_string = NULL;
static size_t Foo_stored_size = 0;

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

	if (Foo_stored_string != NULL)
		kfree(Foo_stored_string);

	printk(KERN_INFO "Cleaning up DebugFS test module.\n");
}

static ssize_t id_file_read(struct file *filp, char *buffer,
			    size_t length, loff_t * offset)
{
	int retval;
	size_t size_read;

	if (*offset >= LOGIN_SIZE) {
		return 0;
	}

	size_read =
	    length < LOGIN_SIZE - *offset ? length : LOGIN_SIZE - *offset;

	retval = copy_to_user(buffer, LOGIN + *offset, size_read);

	if (retval < 0)
		return retval;

	size_read -= retval;
	*offset += size_read;

	return size_read;
}

static ssize_t id_file_write(struct file *filp, const char *buffer,
			     size_t length, loff_t * offset)
{
	char local_buffer[LOGIN_SIZE];

	if (length != LOGIN_SIZE)
		return -EINVAL;

	if (copy_from_user(local_buffer, buffer, LOGIN_SIZE))
		return -EINVAL;

	if (memcmp(local_buffer, LOGIN, LOGIN_SIZE))
		return -EINVAL;

	return LOGIN_SIZE;
}

static ssize_t jiffies_file_read(struct file *filp, char *buffer,
				 size_t length, loff_t * offset)
{
	int size = snprintf(buffer, length, "%lu\n", jiffies);

	return size > length ? length : size;
}

// TODO: add mutex to avoid use after free when read and write at the same time

static ssize_t foo_file_read(struct file *filp, char *buffer,
			     size_t length, loff_t * offset)
{
	int retval;
	size_t read_size;
	if (Foo_stored_string == NULL)
		return 0;

	read_size = (Foo_stored_size - *offset) >
	    length ? length : (Foo_stored_size - *offset);

	retval = copy_to_user(buffer, Foo_stored_string + *offset, read_size);

	if (retval < 0)
		return retval;

	read_size -= retval;

	*offset += read_size;

	return read_size;
}

static ssize_t foo_file_write(struct file *filp, const char *buffer,
			      size_t length, loff_t * offset)
{
	char *new_string;

	if (length > PAGE_SIZE)
		return -EINVAL;

	new_string = kmalloc(length, GFP_KERNEL);

	if (copy_from_user(new_string, buffer, length)) {
		kfree(new_string);
		return -EINVAL;
	}

	kfree(Foo_stored_string);
	Foo_stored_string = new_string;
	Foo_stored_size = length;

	return length;
}

module_init(hello_init);
module_exit(hello_cleanup);
