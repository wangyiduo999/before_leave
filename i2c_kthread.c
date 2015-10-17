#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/i2c.h>
#include <linux/delay.h>

struct sensor {
	struct task_struct *kthrd_handle;
	struct i2c_client client;
};
static struct sensor sensor;

static int kthrd(void *arg)
{
	struct sensor *s = arg;
	s32 ret;

	while (!kthread_should_stop()) {
		ret = i2c_smbus_read_byte(&s->client);
		if (ret >= 0)
			pr_err("%s: i2c_smbus_read_byte returned %x\n", __func__, ret);
		msleep_interruptible(1000);
	}
	return 0;
}

static int __init mod_init(void)
{
	sensor.client.adapter = i2c_get_adapter(0);
	sensor.client.addr = 0x40;
	sensor.kthrd_handle = kthread_run(kthrd, &sensor, "kthrd");
	return 0;
}

static void __exit mod_exit (void)
{
	if (sensor.kthrd_handle)
		kthread_stop(sensor.kthrd_handle);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
