// exmutex.c

#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

#define DEVICE_NAME "exmutex"

#define NCPU 4
static struct work_struct work[NCPU];

// Counters
static volatile int counter_volatile = 0;		// No guard
static volatile int counter_guarded = 0;		// Guarded by mutex
static atomic_t counter_atomic = ATOMIC_INIT(0);	// Atomic object

DEFINE_MUTEX(mutex);

// Process context
static void exmutex_work(struct work_struct *work)
{
    unsigned long end_time = jiffies + HZ;  // Stop in one second
    while (jiffies < end_time) {
        // Count up without protection
        counter_volatile++;

        // Count up with protection
        mutex_lock(&mutex);
        counter_guarded++;
        mutex_unlock(&mutex);

        // Count up atomic
	atomic_inc(&counter_atomic);
    }
}

static int exmutex_init(void)
{
    int i;

    // Run the processes
    for (i = 0; i < NCPU; i++) {
        INIT_WORK(&work[i], exmutex_work);
    }
    for (i = 0; i < NCPU; i++) {
        bool result = schedule_work_on(i, &work[i]);
        if (!result) {
            trace_printk("schedule_work_on(%d) failed\n", i);
            return -EPERM;
        }
    }

    // Wait for completion
    flush_scheduled_work();

    // Show result in /sys/kernel/debug/tracing/trace
    trace_printk("counter: %d < %d == %d\n",
                 counter_volatile, counter_guarded, atomic_read(&counter_atomic));
    return 0;
}

static void exmutex_exit(void)
{
}

module_init(exmutex_init);
module_exit(exmutex_exit);
MODULE_LICENSE("Dual MIT/GPL");
