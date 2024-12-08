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
	debug("State Timestamp: %d\n", state->buf_timestamp);
	debug("Last Update: %d\n", sensor->msr_data[state->type]->last_update);
	return state->buf_timestamp != sensor->msr_data[state->type]->last_update; /*Return 0 if no refresh needed*/
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
		
	debug("entering\n");

	/*
	 * Grab the raw data quickly, hold the
	 * spinlock for as little as possible.
	 */
	/* ? */
	/* Why use spinlocks? See LDD3, p. 119 */

	/*Added by us - Start*/
	WARN_ON (!(sensor = state->sensor));
	uint32_t raw_data;
	spin_lock(&sensor->lock);	
	/*Added by us - End*/

	/*
	 * Any new data available?
	 */
	/* ? */
	
	/*Added by us - Start*/
	if (!lunix_chrdev_state_needs_refresh(state)) { /*If no refresh needed*/
		spin_unlock(&sensor->lock);
		debug("returned no refresh\n");
		return -EAGAIN;
	}
	raw_data = sensor->msr_data[state->type]->values[0]; /*Get new value of sensor*/
	state->buf_timestamp = sensor->msr_data[state->type]->last_update;	/*Get timestamp of new value*/
	/*Added by us - End*/

	/*
	 * Now we can take our time to format them,
	 * holding only the private state semaphore
	 */

	/* ? */
	
	/*Added by us - Start*/
	spin_unlock(&sensor->lock);
	long cooked_data;
	switch(state->type) {/*Format values according to lookup table*/
		case BATT: 
			cooked_data = lookup_voltage[raw_data]; break;
		case TEMP: 
			cooked_data = lookup_temperature[raw_data]; break;
		case LIGHT: 
			cooked_data = lookup_light[raw_data]; break;
		default: 
			debug("unknown type\n"); break;
	}
	state->buf_lim = snprintf(state->buf_data, LUNIX_CHRDEV_BUFSZ, "%ld.%03ld\n", cooked_data/1000, cooked_data%1000); /*Put formated values in buffer*/
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
	struct lunix_chrdev_state_struct *state;
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
	state = kmalloc(sizeof(struct lunix_chrdev_state_struct), GFP_KERNEL); /*Allocate necessary memory for state struct*/
    	if (!state) {
		debug("kmalloc: could not allocate requested memory\n");
        	ret = -ENOMEM;
        	goto out;
    	}
	state->type = measurement_type; /*Type of measurement*/
	state->sensor = &lunix_sensors[sensor_id]; /*state->sensor points to lunix sensor with specific id*/
	state->buf_lim = 0; /*Initialize buffer size*/
	state->buf_timestamp = 0; /*Initialize buffer timestamp*/
	sema_init(&state->lock, 1); /*Initialize semaphore*/
	filp->private_data = state; /*filp->private points to newly created memory area*/
	ret = 0;
	/*Added by us - End*/

out:
	debug("leaving, with ret = %d\n", ret);
	return ret;
}

static int lunix_chrdev_release(struct inode *inode, struct file *filp)
{
	/* ? */
	
	/*Added by us - Start*/
	struct lunix_chrdev_state_struct *state;
	state = filp->private_data;
	if (state) {
		kfree(state); /*Free allocated memory*/
        	filp->private_data = NULL; /*Do not create dangling pointer*/
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

	debug("entering\n");
		
	state = filp->private_data;
	WARN_ON(!state);

	sensor = state->sensor;
	WARN_ON(!sensor);

	/* Lock? */

	/*Added by us - Start*/
	if(down_interruptible(&state->lock)) { /*Semaphore flag down*/
		debug("returned -ERESTARTSYS 1\n");
		return -ERESTARTSYS;
	}	
	/*Added by us - End*/

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

			/*Added by us - Start*/
			up(&state->lock); /*Semaphore flag up*/	
			if(wait_event_interruptible(sensor->wq, lunix_chrdev_state_needs_refresh(state))) { /*Sleep until condition becomes 1 or interrupted*/
				debug("returned -ERESTARTSYS 2\n");
				return -ERESTARTSYS;
			}
			if(down_interruptible(&state->lock)) { /*Semaphore flag down*/
				debug("returned -ERESTARTSYS 3\n");
				return -ERESTARTSYS;
			}	
			/*Added by us - End*/
		}
		debug("leaving while\n");
	}
	
	/* End of file */
	/* ? */

	/*Added by us - Start*/
	size_t remain_bytes = state->buf_lim - *f_pos; /*Check how many bytes available to be read before exceeding size of buffer*/
	if (cnt > remain_bytes) cnt = remain_bytes; /*If exceeds buffer size, read only the remaining bytes instead*/
	/*Added by us - End*/

	/* Determine the number of cached bytes to copy to userspace */
	/* ? */
	
	/*Added by us - Start*/
	if(copy_to_user(usrbuf, (state->buf_data + *f_pos), cnt)) {/*Safely transfer data to user space*/
		up(&state->lock); /*Semaphore flag up*/	
		debug("returned -ERESTARTSYS 4\n");
		return -EFAULT;
	}
	*f_pos += cnt; /*Increase "file descriptor"*/
	ret = cnt; /*Return how many bytes read*/
	/*Added by us - End*/

	/* Auto-rewind on EOF mode? */
	/* ? */

	/*Added by us - Start*/
	if (*f_pos >= state->buf_lim) *f_pos = 0; /*If reached file limit, move "file descriptor" to the beggining again*/
	/*Added by us - End*/
	
	/*
	 * The next two lines  are just meant to suppress a compiler warning
	 * for the "unused" out: label, and for the uninitialized "ret" value.
	 * It's true, this helpcode is a stub, and doesn't use them properly.
	 * Remove them when you've started working on this code.
	 */
	//ret = -ENODEV;
	goto out;
out:
	/* Unlock? */

	/*Added by us - Start*/	
	up(&state->lock); /*Semaphore flag up*/
	debug("leaving\n");	
	/*Added by us - End*/

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
	ret = register_chrdev_region(dev_no, lunix_minor_cnt, "lunix"); /*register a range of device numbers (major and minor) for a lunix device driver*/	
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
	ret = cdev_add(&lunix_chrdev_cdev, dev_no, lunix_minor_cnt); /*add a cdev structure (character device) to the system, associating it with a range of device numbers*/
	/*Added by us - End*/
	
	if (ret < 0) {
		debug("failed to add character device\n");
		goto out_with_chrdev_region;
	}
	debug("completed successfully\n");
	goto out;

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
