#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/input.h>

#define KEY_DEV "/dev/input/event0"
#define MOU_DEV "/dev/input/event6"
#define TOUS_DEV "/dev/input/event7"
#define TOUP_DEV "/dev/input/event11"

static int ts_fd = -1;
static struct input_event data;


static int init_device(char *TS_DEV)
{
    if((ts_fd = open(TS_DEV, O_RDONLY)) < 0)
    {
        printf("Error open %s\n\n", TS_DEV);
        return -1;
    }
    return ts_fd;
}

static int test_key()
{
    if(init_device(KEY_DEV) < 0)
        return -1;
    while(1)
    {
        read(ts_fd, &data, sizeof(data));
        if (data.type == EV_KEY)
            printf("  type: EV_KEY, event = %d, value = %d\n",data.code, data.value);
    }
    return 0;
}

static int test_mouse()
{
    if(init_device(MOU_DEV) < 0)
        return -1;
    while(1)
    {
        read(ts_fd, &data, sizeof(data));
        if (data.type == EV_KEY)
        {
            printf("  type = EV_KEY, code = %s, value = %d\n",
                data.code == BTN_LEFT ? "MOUSE_LEFT" :
                data.code == BTN_RIGHT ? "MOUSE_RIGHT" :
                data.code == BTN_MIDDLE ? "MOUSE_MIDDLE" :
                data.code == BTN_SIDE ? "MOUSE_SIDE" :
                "Unkonw", data.value);
        }
        else if(data.type == EV_REL)
        {
            printf("  type = EV_ABS, code = %s, value = %d\n",
                data.code == REL_X ? "ABS_X" :
                data.code == REL_Y ? "ABS_Y" :
                data.code == ABS_WHEEL ? "MOUSE_WHEEL" :
                data.code == ABS_PRESSURE ? "ABS_PRESSURE" :
                "Unkown", data.value);
        }
    }
    return 0;
}

static int test_touch_screen()
{
    if(init_device(TOUS_DEV) < 0)
        return -1;
    while(1)
    {
        read(ts_fd, &data, sizeof(data));
        if (data.type == EV_KEY)
        {
            printf("  type: EV_KEY, event = %s, value = %d\n\n",
                data.code == BTN_TOUCH ? "BTN_TOUCH" : "Unkown", data.value);
        }
        else if(data.type == EV_ABS)
        {
            printf("  type: EV_ABS, event = %s, value = %d\n\n",
                data.code == ABS_X ? "ABS_X" :
                data.code == ABS_Y ? "ABS_Y" :
                data.code == ABS_PRESSURE ? "ABS_PRESSURE" :
                "Unkown", data.value);
        }
    }
    return 0;
}

static int test_touch_pancel()
{
    if(init_device(TOUP_DEV) < 0)
        return -1;
    while(1)
    {
        read(ts_fd, &data, sizeof(data));
        if (data.type == EV_KEY)
        {
            printf("  type = EV_KEY, code = %s, value = %d\n",
                data.code == BTN_LEFT ? "MOUSE_LEFT" :
                data.code == BTN_RIGHT ? "MOUSE_RIGHT" :
                data.code == BTN_MIDDLE ? "MOUSE_MIDDLE" :
                data.code == BTN_SIDE ? "MOUSE_SIDE" :
                "Unkonw", data.value);
        }
        else if(data.type == EV_REL)
        {
            printf("  type = EV_ABS, code = %s, value = %d\n",
                data.code == REL_X ? "ABS_X" :
                data.code == REL_Y ? "ABS_Y" :
                data.code == ABS_WHEEL ? "MOUSE_WHEEL" :
                data.code == ABS_PRESSURE ? "ABS_PRESSURE" :
                "Unkown", data.value);
        }
    }
    return 0;
}

int main()
{
    static int i;
select:   
	printf("Please select device:\n0.KeyBoard\n1.Mouse\n2.TouchScreen\n3.TouchPancel\n");
    scanf("%d",&i);
    switch(i){
        case 0:
            test_key();
            break;
        case 1:
            test_mouse();
            break;
        case 2:
            test_touch_screen();
            break;
        case 3:
            test_touch_pancel();
            break;
        default:
            printf("Wrong device, Please select again!\n\n");
            break;       
    }
    goto select;
        return 0;
}






