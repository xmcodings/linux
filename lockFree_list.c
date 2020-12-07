#define NUMBER_OF_DATA 100000
#define NUMBER_OF_THREAD 1 
#define THREAD_WORK 100000

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h> //kmalloc

#include <linux/list.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/delay.h>

struct kernel_node{
	struct list_head list;
	int data;
};

struct node{
	struct node *next;
	int data;
};

struct list_head klist;

struct node *head;
struct node *tail;

ktime_t start, end;
s64 kernelD_time, improveD_time;

int insertThread(void *arg){
	int i;
	struct node *temp;

	start = ktime_get();
	for(i=0; i< THREAD_WORK; i++){
		struct node *new = kmalloc(sizeof(struct kernel_node),GFP_KERNEL);
		new->data = i;
		new->next = NULL;
		temp =  __sync_lock_test_and_set(&tail, new);
		__sync_lock_test_and_set(&temp->next, new);
	}
	end = ktime_get();
	__sync_fetch_and_add(&improveD_time, ktime_to_ns(ktime_sub(end, start)));
	return 0;
}

void compareInsert(void){
	/* kernel data structure */
	int i;
	INIT_LIST_HEAD(&klist);
	head = kmalloc(sizeof(struct kernel_node), GFP_KERNEL);
	head->data = -1;
	tail = head;

	improveD_time = 0;

	start = ktime_get();
	for(i=0; i<NUMBER_OF_DATA; i++){
		struct kernel_node *new = kmalloc(sizeof(struct kernel_node),GFP_KERNEL);
		new->data = i;
		list_add_tail(&new->list, &klist);
	}
	end = ktime_get();
	kernelD_time = ktime_to_ns(ktime_sub(end, start));
	printk("---INSERT time---\nKernel data structure -> %lld ns",(long long)kernelD_time);
		
	/* thread data structure */
	for(i=0; i<NUMBER_OF_THREAD; i++)
	{
		kthread_run(&insertThread, NULL,"insertThread");
	}
	msleep(3000); // 쓰레드 다 끝날 때가지 대기

	printk("Improve data structure -> %lld ns\n",(long long)improveD_time);
	printk("Improve %lld ns\n\n",(long long)(kernelD_time-improveD_time));
}


static int __init list_module_init(void)
{
	printk("Improve Data structure performance! Data %d.\n", NUMBER_OF_DATA);
	compareInsert();
	//compareDelete();
	//compareTraverse();
	return 0;
}

static void __exit list_module_cleanup(void)
{
	printk("We are team 8!\n");
}

module_init(list_module_init);
module_exit(list_module_cleanup);
MODULE_LICENSE("GPL");
