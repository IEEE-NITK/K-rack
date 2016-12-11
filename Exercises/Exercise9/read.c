#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/cpufreq.h>

ssize_t read( struct file * filep,
    char * fname, size_t sizeval, loff_t * loffset )
{
    static int finished = 0;
    int processed_bytes = 0;
    /*
     *  processed_bytes variable keeps track of the
        number of bytes read successfully.
     *  It is also the return value of the read
        function.
     *  sizeval is the number of bytes to be read.
     *  Loop until all the requested bytes are provided
        or we run out of content to provide.
     *  put_user is a macro that takes a value and copies
        it to a destination in the user space.
     */

    char message[200];
    sprintf(message,"CPU0:%d\nCPU1:%d\nCPU2:%d\nCPU3:%d\n",
                    cpufreq_get(0),
                    cpufreq_get(1),
                    cpufreq_get(2),
                    cpufreq_get(3));
                    /*
                     *  From the official documentation
                     *  cpufreq_get - get the current CPU frequency (in kHz)
                     *  @cpu: CPU number
                     *
                     *  Get the CPU current (static) CPU frequency
                     *  Definition
                        unsigned int cpufreq_get(unsigned int cpu)
                        {
                        	struct cpufreq_policy *policy = cpufreq_cpu_get(cpu);
                         	unsigned int ret_freq = 0;

                         	if (policy) {
                         		down_read(&policy->rwsem);
                         		ret_freq = __cpufreq_get(policy);
                         		up_read(&policy->rwsem);

                         		cpufreq_cpu_put(policy);
                         	}

                         	return ret_freq;
                        }
                        EXPORT_SYMBOL(cpufreq_get);
                     *  Declaration
                     */

    /*
     *  This is the part that returns the message combined with the
     *  frequency at each calling time.
     *  I have four cores hence there are four cpufreq_get calls.
     *  The calls can be found at drivers/cpufreq/cpufreq.c
     *  which is prototyped at linux/cpufreq.h
     */
    char *buffer_ptr = message;
    if(finished)
    {
        finished = 0;
        return 0;
    }
    finished = 1;
    while( sizeval && *buffer_ptr )
    {
        put_user(*(buffer_ptr++),fname++);
        sizeval--;
        processed_bytes++;
    }

    /*
     *  (NOTE: By popular convention the number of processed
        bytes are returned. However, it is not mandatory to
        do so).
     */
    return processed_bytes;
}
