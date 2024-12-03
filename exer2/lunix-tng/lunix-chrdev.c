/*
 * lunix-chrdev.c
 *
 * Implementation of character devices
 * for Lunix:TNG
 *
 * < Your name here >
 *
 */

#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mmzone.h>
#include <linux/vmalloc.h>
#include <linux/spinlock.h>

#include "lunix.h"
#include "lunix-chrdev.h"
#include "lunix-lookup.h"

/*
 * Global data
 */
struct cdev lunix_chrdev_cdev;

/*
 * Just a quick [unlocked] check to see if the cached
 * chrdev state needs to be updated from sensor measurements.
 */
/*
 * Declare a prototype so we can define the "unused" attribute and keep
 * the compiler happy. This function is not yet used, because this helpcode
 * is a stub.
 */
static int __attribute__((unused)) lunix_chrdev_state_needs_refresh(struct lunix_chrdev_state_struct *);
static int lunix_chrdev_state_needs_refresh(struct lunix_chrdev_state_struct *state)
{
	struct lunix_sensor_struct *sensor;
	
	WARN_ON ( !(sensor = state->sensor));
	/* ? */
	
	/*Added by us - Start*/
		if (state->buf_timestamp == sensor->msr_data[state->type]->last_update)
		return 0;
	else
		return 1;
	/*Added by us - End*/

	/* The following return is bogus, just for the stub to compile */
	/*return 0;*/ /* ? */
}

/*
 * Updates the cached state of a character device
 * based on sensor data. Must be called with the
 * character device state lock held.
 */
static int lunix_chrdev_state_update(struct lunix_chrdev_state_struct *state)
{
	struct lunix_sensor_struct __attribute__((unused)) *sensor;
		
	debug("leaving\n");

	/*
	 * Grab the raw data quickly, hold the
	 * spinlock for as little as possible.
	 */
	/* ? */
	/* Why use spinlocks? See LDD3, p. 119 */

	/*Added by us - Start*/
	WARN_ON ( !(sensor = state->sensor));
	uint32_t raw_data 
	spin_lock(&sensor->lock);	
	/*Added by us - End*/

	/*
	 * Any new data available?
	 */
	/* ? */
	
	/*Added by us - Start*/
	if (!lunix_chrdev_state_needs_refresh(state)) {
		spin_unlock(&sensor->lock);
		return -EAGAIN;
	}
	raw_data = &sensor->msr_data[state->type]->values[0];
	state->buf_timestamp = sensor->msr_data[state->type]->last_update;
	
	/*Added by us - End*/

	/*
	 * Now we can take our time to format them,
	 * holding only the private state semaphore
	 */

	/* ? */
	
	/*Added by us - Start*/
	spin_unlock(&sensor->lock);
	down(&state->lock);
	state->buf_lim = snprintf(state->buf_data, LUNIX_CHRDEV_BUFSZ, "%u\n", raw_data);
	up(&state->lock);
	/*Added by us - End*/

	debug("leaving\n");
	return 0;
}

/*************************************
 * Implementation of file operations
 * for the Lunix character device
 *************************************/

static int lunix_chrdev_open(struct inode *inode, struct file *filp)
{
	/* Declarations */
	/* ? */
	
	/*Added by us - Start*/
	unsigned int minor_num = iminor(inode);
	struct lunix_chrdev_state_struct dev;
	unsigned int sensor_id = minor_num / 8;
	unsigned int measurement_type = minor_num % 8;
	/*Added by us - End*/

	int ret;

	debug("entering\n");
	ret = -ENODEV;
	if ((ret = nonseekable_open(inode, filp)) < 0)
		goto out;

	/*
	 * Associate this open file with the relevant sensor based on
	 * the minor number of the device node [/dev/sensor<NO>-<TYPE>]
	 */
	
	/* Allocate a new Lunix character device private state structure */
	/* ? */

	/*Added by us - Start*/
	dev = kmalloc(sizeof(struct lunix_chrdev_state_struct), GFP_KERNEL);
    	if (!dev) {
        	ret = -ENOMEM;
        	goto out;
    	}
	dev->type = measurement_type;
	dev->sensor = &lunix_sensors[sensor_id];
	dev->buf_lim = 0;
	memset(dev->buf_data, 0, LUNIX_CHRDEV_BUFSZ);
	sema_init(&dev->lock, 1);
	filp->private_data = dev;
	/*Added by us - End*/

out:
	debug("leaving, with ret = %d\n", ret);
	return ret;
}

static int lunix_chrdev_release(struct inode *inode, struct file *filp)
{
	/* ? */
	
	/*Added by us - Start*/
	struct lunix_chrdev_state_struct *dev;
	dev = filp->private_data;
	if (dev) {
		kfree(dev);
        	filp->private_data = NULL;
	}
	/*Added by us - End*/


	return 0;
}

static long lunix_chrdev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	return -EINVAL;
}

static ssize_t lunix_chrdev_read(struct file *filp, char __user *usrbuf, size_t cnt, loff_t *f_pos)
{
	ssize_t ret;

	struct lunix_sensor_struct *sensor;
	struct lunix_chrdev_state_struct *state;

	state = filp->private_data;
	WARN_ON(!state);

	sensor = state->sensor;
	WARN_ON(!sensor);

	/* Lock? */
	/*
	 * If the cached character device state needs to be
	 * updated by actual sensor data (i.e. we need to report
	 * on a "fresh" measurement, do so
	 */
	if (*f_pos == 0) {
		while (lunix_chrdev_state_update(state) == -EAGAIN) {
			/* ? */
			/* The process needs to sleep */
			/* See LDD3, page 153 for a hint */
		}
	}

	/* End of file */
	/* ? */
	
	/* Determine the number of cached bytes to copy to userspace */
	/* ? */

	/* Auto-rewind on EOF mode? */
	/* ? */

	/*
	 * The next two lines  are just meant to suppress a compiler warning
	 * for the "unused" out: label, and for the uninitialized "ret" value.
	 * It's true, this helpcode is a stub, and doesn't use them properly.
	 * Remove them when you've started working on this code.
	 */
	ret = -ENODEV;
	goto out;
out:
	/* Unlock? */
	return ret;
}

static int lunix_chrdev_mmap(struct file *filp, struct vm_area_struct *vma)
{
	return -EINVAL;
}

static struct file_operations lunix_chrdev_fops = 
{
	.owner          = THIS_MODULE,
	.open           = lunix_chrdev_open,
	.release        = lunix_chrdev_release,
	.read           = lunix_chrdev_read,
	.unlocked_ioctl = lunix_chrdev_ioctl,
	.mmap           = lunix_chrdev_mmap
};

int lunix_chrdev_init(void)
{
	/*
	 * Register the character device with the kernel, asking for
	 * a range of minor numbers (number of sensors * 8 measurements / sensor)
	 * beginning with LINUX_CHRDEV_MAJOR:0
	 */
	int ret;
	dev_t dev_no;
	unsigned int lunix_minor_cnt = lunix_sensor_cnt << 3;

	debug("initializing character device\n");
	cdev_init(&lunix_chrdev_cdev, &lunix_chrdev_fops);
	lunix_chrdev_cdev.owner = THIS_MODULE;
	
	dev_no = MKDEV(LUNIX_CHRDEV_MAJOR, 0);
	/* ? */
	/* register_chrdev_region? */
	
	/*Added by us - Start*/
	ret = register_chrdev_region(dev_no, lunix_minor_cnt, "lunix");	
	/*Added by us - End*/

	/* Since this code is a stub, exit early */
	//return 0;

	if (ret < 0) {
		debug("failed to register region, ret = %d\n", ret);
		goto out;
	}
	/* ? */
	/* cdev_add? */
	
	/*Added by us - Start*/
	ret = cdev_add(&lunix_chrdev_cdev, dev_no, lunix_minor_cnt);
	/*Added by us - End*/
	
	if (ret < 0) {
		debug("failed to add character device\n");
		goto out_with_chrdev_region;
	}
	debug("completed successfully\n");
	return 0;

out_with_chrdev_region:
	unregister_chrdev_region(dev_no, lunix_minor_cnt);
out:
	return ret;
}

void lunix_chrdev_destroy(void)
{
	dev_t dev_no;
	unsigned int lunix_minor_cnt = lunix_sensor_cnt << 3;

	debug("entering\n");
	dev_no = MKDEV(LUNIX_CHRDEV_MAJOR, 0);
	cdev_del(&lunix_chrdev_cdev);
	unregister_chrdev_region(dev_no, lunix_minor_cnt);
	debug("leaving\n");
}
