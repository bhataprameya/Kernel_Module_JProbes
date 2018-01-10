#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/mm_types.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/coda.h>
#define size 40
static int pid=11;
module_param(pid,int,S_IRUGO); 
int writecount=0,reatcount=0;
int openfns(struct inode *inode,struct file *filep);
int closefns(struct inode *inode,struct file *filep);
ssize_t readfns(struct file *filep,char *buff,size_t count,loff_t *offp );

struct timespec ts;
typedef struct processes {
	unsigned long address[size];
	long nsec[size];
	time_t sec[size];

}process_data;
process_data data;

//Keeps track of all the file operations in this character device
struct file_operations my_fops={
	.owner = THIS_MODULE,
	.open= openfns,
	.read= readfns,
	.release=closefns
};

static int __handle_mm_fault(struct vm_area_struct *vma, unsigned long address,unsigned int flags)
{ 
	if(current->pid==pid)
	{	
		//clock_gettime(CLOCK_REALTIME, &ts);
		getnstimeofday(&ts);
		printk(KERN_INFO "Virtual Address= 0x%lX Time=%ld.%ld \n",address,ts.tv_sec,ts.tv_nsec);
		//printk(KERN_INFO " %ld.%ld, %lX \n",ts.tv_sec,ts.tv_nsec,address);		
		data.address[writecount]=address;
		data.sec[writecount]=ts.tv_sec;
		data.nsec[writecount]=ts.tv_nsec;
		writecount++;
		writecount=writecount%size;
	}
	jprobe_return();
	return 0;
}

static struct jprobe my_jprobe = 
{
	.entry= __handle_mm_fault,
	.kp = 
	{
		.symbol_name= "handle_mm_fault",
	},
};

//Caled then the character device is opened
int openfns(struct inode *inode,struct file *filep)
{ 
	try_module_get(THIS_MODULE);
	return 0;
}
//Called when character device is closed
int closefns(struct inode *inode,struct file *filep)
{
	module_put(THIS_MODULE);
	return 0;
}

//Called when Read call is invoked
ssize_t readfns(struct file *filep,char *buffer,size_t count,loff_t *offp )
{

	if (copy_to_user(buffer,&data,sizeof(process_data)) != 0 )
	{
		printk( "Kernel -> userspace copy failed!\n" );
		return 0;
	}
	return sizeof(process_data);
}

static int __init jprobe_init(void)
{
	int ret;
	//proc_create("pagefault", 0, NULL, &my_fops);
	ret = register_jprobe(&my_jprobe);
	if (ret < 0) {
		printk(KERN_INFO "register_jprobe failed, returned %d\n", ret);
		return -1;
	}
	proc_create("pagefault", 0, NULL, &my_fops);
	printk(KERN_INFO "Planted jprobe at %p, handler addr %p\n",
			my_jprobe.kp.addr, my_jprobe.entry);
	return 0;
}

static void __exit jprobe_exit(void)
{
	remove_proc_entry("pagefault", NULL);
	unregister_jprobe(&my_jprobe);
	//remove_proc_entry("hello_proc", NULL);	
	printk(KERN_INFO "jprobe at %p unregistered\n", my_jprobe.kp.addr);
}

	module_init(jprobe_init)
module_exit(jprobe_exit)
	MODULE_LICENSE("GPL");
