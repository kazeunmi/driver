// exspinlock.c

#include <linux/module.h>
#include <linux/hrtimer.h>

#define DEVICE_NAME "exspinlock"
#define TIMER_INTERVAL_US 10000

// Counters
static volatile int counter_volatile = 0;		// No guard
static volatile int counter_guarded = 0;		// Guarded by spinlock
static atomic_t counter_atomic = ATOMIC_INIT(0);	// Atomic object
DEFINE_MUTEX(mutex);
DEFINE_SPINLOCK(lock);

// High-resolution timer
static struct hrtimer timer;
static ktime_t kt_period;

static enum hrtimer_restart exspinlock_timer_handler(struct hrtimer* ptimer)
{
    // Count up without protection
    counter_volatile++;

    // Count up with protection
    atomic_inc(&counter_atomic);
    spin_lock(&lock);
    counter_guarded++;
    spin_unlock(&lock);

    // Respawn timer
    hrtimer_forward_now(ptimer, kt_period);
    return HRTIMER_RESTART;
}

static int exspinlock_init(void)
{
    unsigned long flags;
    unsigned long end_time = jiffies + HZ;  // Stop in one second

    // Start timer
    kt_period = ktime_set(0, TIMER_INTERVAL_US);
    hrtimer_init(&timer, CLOCK_REALTIME, HRTIMER_MODE_REL);
    timer.function = exspinlock_timer_handler;
    hrtimer_start(&timer, kt_period, HRTIMER_MODE_REL);

    // Count for one second
    while (jiffies < end_time) {
        // Count up without protection
        counter_volatile++;

        // Count up with protection
        atomic_inc(&counter_atomic);
        spin_lock_irqsave(&lock, flags);
        counter_guarded++;
        spin_unlock_irqrestore(&lock, flags);
    }

    // Make sure timer is completed
    hrtimer_cancel(&timer);

    // Show result in /sys/kernel/debug/tracing/trace
    trace_printk("counter: %d < %d == %d\n",
                 counter_volatile, counter_guarded, atomic_read(&counter_atomic));
    return 0;
}

static void exspinlock_exit(void)
{
}

module_init(exspinlock_init);
module_exit(exspinlock_exit);
MODULE_LICENSE("Dual MIT/GPL");
