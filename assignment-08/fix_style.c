#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_LICENSE("LICENSE");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

static ssize_t myfd_read(struct file *fp, char __user * user, size_t size,
			 loff_t * offs);
static ssize_t myfd_write(struct file *fp, const char __user * user,
			  size_t size, loff_t * offs);

static struct file_operations myfd_fops = {
	.owner = THIS_MODULE,
	.read = &myfd_read,
	.write = &myfd_write
};

static struct miscdevice myfd_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "reverse",
	.fops = &myfd_fops
};

char str[PAGE_SIZE];

static int __init myfd_init(void)
{
	int retval;

	retval = misc_register(&myfd_device);

	str[0] = 0;
	if (retval) {
		printk(KERN_ERR "misc_register returned: %d\n", retval);
		return retval;
	}

	return 0;
}

static void __exit myfd_cleanup(void)
{
	misc_deregister(&myfd_device);
}

static ssize_t myfd_read(struct file *fp, char __user * user, size_t size,
			 loff_t * offs)
{
	ssize_t t, i;
	char *rev_str;
	int retval;
	ssize_t str_size;

	str_size = strlen(str);

	rev_str = kmalloc(PAGE_SIZE, GFP_KERNEL);

	printk(KERN_WARNING "str_size: %ld\n", str_size);

	for (t = str_size - 1, i = 0; t >= 0; t--, i++) {
		printk(KERN_ERR "t: %ld", t);
		rev_str[i] = str[t];
	}

	rev_str[i] = 0;

	retval = simple_read_from_buffer(user, size, offs, rev_str, i);

	kfree(rev_str);

	return retval;
}

static ssize_t myfd_write(struct file *fp, const char __user * user,
			  size_t size, loff_t * offs)
{
	ssize_t res;

	if (size + 1 >= PAGE_SIZE) {
		return -1;
	}
	res = simple_write_to_buffer(str, size, offs, user, size);
	str[res + 1] = 0;

	return res + 1;
}

module_init(myfd_init);
module_exit(myfd_cleanup)
