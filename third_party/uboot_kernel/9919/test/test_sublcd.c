#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/ioctl.h>



#define ATCOM_SUB_LCD_NAME "/dev/fb1"
#define ARRAY_SIZE(X) (sizeof(X)/sizeof(X[0]))

#define ATCOM_LCD_IOW(num, dtype)         _IOW('U', num, dtype)
#define ATCOM_LCD_IOR(num, dtype)         _IOR('U', num, dtype)
#define ATCOM_LCD_IOWR(num, dtype)        _IOWR('U', num, dtype)
#define ATCOM_LCD_IO(num)                 _IO('U', num)
#define UPDATE_LCD_SCREEN             	 ATCOM_LCD_IO(31)
#define SHOW_BOOT_PROGRESS			     ATCOM_LCD_IOW(0x07,unsigned int)
#define SET_LCD_CONTRAST				 ATCOM_LCD_IOW(0x03, unsigned char)
#define SET_LCD_BACKLIGHT				 ATCOM_LCD_IOW(0x04, unsigned short)
#define GET_LCD_BACKLIGHT				 ATCOM_LCD_IOW(0x05, unsigned short)
#define SET_LCD_INVERSION_DISPLAY		 ATCOM_LCD_IOW(0x10, unsigned char)

typedef struct atcom_sub_lcd{
	int fd;
	unsigned int fbsize;
	unsigned char * pbuffer;	//picture send buffer,which mmaped .
	unsigned char * pbuf_bak;	//draw picture in this buffer
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
}atcom_sub_lcd_t,*patcom_sub_lcd_t;


unsigned char *cmd_name[]={
	"SET_LCD_CONTRAST",
	"SET_LCD_BACKLIGHT",
	"GET_LCD_BACKLIGHT",
	"SET_LCD_INVERSION_DISPLAY",
	"exit",
};

static int atcom_sub_lcd_set_contrast(patcom_sub_lcd_t pinfo);
static int atcom_sub_lcd_set_backlight(patcom_sub_lcd_t pinfo);
static int atcom_sub_lcd_get_backlight(patcom_sub_lcd_t pinfo);
static int atcom_sub_lcd_set_inversion_display(patcom_sub_lcd_t pinfo);


static int(*test_func[])(patcom_sub_lcd_t pinfo)={
	atcom_sub_lcd_set_contrast,
	atcom_sub_lcd_set_backlight,
	atcom_sub_lcd_get_backlight,
	atcom_sub_lcd_set_inversion_display,
};

unsigned int get_input(void)
{
	unsigned int input = 0;
	scanf("%ud",&input);
	getchar();
	return input;
}

static int atcom_sub_lcd_set_backlight (patcom_sub_lcd_t pinfo)
{
	int ret = 0;
	unsigned short bringhtness = 0;
	if( NULL == pinfo ){
		printf("Error: Param is NULL [%s:%d]!\n",__func__,__LINE__);
		return -1;
	}
	printf("\n please input a lcd's bringhtness value:\n");
	bringhtness = get_input();
	printf("you input value is %d\n",bringhtness);
	ret =ioctl(pinfo->fd,SET_LCD_BACKLIGHT,&bringhtness);
	if( 0 != ret ){
		printf("Error: SET_LCD_BACKLIGHT failed ret=%d!\n",ret);
	}
	return ret;
}

static int atcom_sub_lcd_get_backlight(patcom_sub_lcd_t pinfo)
{
	int ret = 0;
	unsigned short bringhtness = 0;
	if( NULL == pinfo ){
		printf("Error: Param is NULL [%s:%d]!\n",__func__,__LINE__);
		return -1;
	}
	ret =ioctl(pinfo->fd,GET_LCD_BACKLIGHT,&bringhtness);
	if( 0 != ret ){
		printf("Error: GET_LCD_BACKLIGHT failed ret=%d!\n",ret);
	}else{
		printf("\n Sub lcd backlight = %d \n",bringhtness);
	}

	return ret;
}


static int atcom_sub_lcd_set_contrast(patcom_sub_lcd_t pinfo)
{
	int ret = 0;
	unsigned char constrast = 0;
	if( NULL == pinfo ){
		printf("Error: Param is NULL [%s:%d]!\n",__func__,__LINE__);
		return -1;
	}

	printf("\n please input a lcd's contrast value(0~255):\n");
	constrast = (get_input()& 0xFF);
	printf("\n You wanna set contrast is %u\n",constrast);
	ret =ioctl(pinfo->fd,SET_LCD_CONTRAST,&constrast);
	if( 0 != ret ){
		printf("Error: SET_LCD_CONTRAST failed ret=%d!\n",ret);
	}
	return ret;
}



static int atcom_sub_lcd_set_inversion_display(patcom_sub_lcd_t pinfo)
{
	int ret = 0;
	unsigned char inversion = 0;
	unsigned char choose = 0;
	if( NULL == pinfo ){
		printf("Error: Param is NULL [%s:%d]!\n",__func__,__LINE__);
		return -1;
	}
	printf("do you wanna start sub lcd's inversion display function ? yes[Y|y] no[N|n] \n");
	
	
	scanf("%c",&choose);
	getchar();
	
	if( 'Y' == choose || 'y' == choose){
		inversion = 1;
		printf("\n You wanna turn on inversion display function\n");
	}else{
		inversion = 0;
		printf("\n You wanna turn off inversion display function\n");
	}

	ret =ioctl(pinfo->fd,SET_LCD_INVERSION_DISPLAY,&inversion);
	if( 0 != ret ){
		printf("Error: SET_LCD_CONTRAST failed ret=%d!\n",ret);
	}

	return ret;
}


static int atcom_lcd_init(patcom_sub_lcd_t pinfo)
{
	int ret  = 0;
	int lcd_fd 	 = 0;
    long int fbsize = 0;
	char * pbuf				= NULL;
	char * pbuf_b			= NULL;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;


	if( NULL == pinfo ){
		printf("Error:Param is NULL [%s:%d]!\n",__func__,__LINE__);
		return -1;
	}
	
	//open device file description
    lcd_fd = open(ATCOM_SUB_LCD_NAME,O_RDWR);
	if( lcd_fd < 0 ){
		printf("Error: open %s failed  ret = %d [%s:%d]!",ATCOM_SUB_LCD_NAME,lcd_fd,__func__,__LINE__);
		return -1;
	}

	pinfo->fd = lcd_fd;

	//init device info	
    if (ioctl(lcd_fd, FBIOGET_FSCREENINFO, &finfo)){
        printf("Error: reading fixed information.\n");
        goto error1;
    }

	memcpy(&(pinfo->finfo),&finfo,sizeof(finfo));


    if (ioctl(lcd_fd, FBIOGET_VSCREENINFO, &vinfo)){
        printf("Error: reading variable information.\n");
        goto error1;
    }
	memcpy(&(pinfo->vinfo),&vinfo,sizeof(vinfo));
	

    printf("finfo.id = %s\n", finfo.id);
    printf("vinfo.xres=%d\n", vinfo.xres);
    printf("vinfo.yres=%d\n", vinfo.yres);
    printf("vinfo.bits_per_bits=%d\n", vinfo.bits_per_pixel);
    printf("finfo.line_length=%d\n", finfo.line_length);
	
	// Size of frame buffer
	fbsize =vinfo.yres*((vinfo.xres*vinfo.bits_per_pixel+7)/8);

	pinfo->fbsize = fbsize;

	if ((pbuf =(char *) mmap(0, fbsize, PROT_READ | PROT_WRITE,MAP_SHARED,lcd_fd, 0)) == (void *) -1) {
		printf("Error: lcd frame buffer mapped failed [%s:%d]!\n",__func__,__LINE__);
		goto error1;
	}

	pinfo->pbuffer = pbuf;

	pbuf_b =(char *)malloc(fbsize);
	if( NULL == pbuf_b )
	{
		printf("Error:malloc picture back buffer failed [%s:%d]!\n",__func__,__LINE__);
		goto error2;
	}
	pinfo->pbuf_bak = pbuf_b;

	return 0;

error2:
	if(NULL != pbuf){
		munmap(pbuf,fbsize);
	}
error1:
	close(lcd_fd);
error:
	return -1;

}



static void atcom_lcd_uninit(patcom_sub_lcd_t pinfo)
{
	if( NULL == pinfo ){
		return ;
	}

	if(NULL != pinfo->pbuffer){
		munmap(pinfo->pbuffer,pinfo->fbsize);
	}

	if( NULL != pinfo->pbuf_bak ){
		free(pinfo->pbuf_bak);
	}

	close(pinfo->fd);

	return ;
}


static int atcom_lcd_draw_picture(patcom_sub_lcd_t pinfo)
{
	int 	ret = 0;
	
	return 0;
}

static void atcom_print_cmd(patcom_sub_lcd_t pinfo)
{
	int index 	= 0;
	int i		= 0;
	printf("Please choose a cmd:\n");
	for( i = 0 ; i < ARRAY_SIZE(cmd_name) ; i++ ){
		printf("%d):%s\n",i,cmd_name[i]);
	}
	printf("\n");
}


static int atcom_choose_cmd(patcom_sub_lcd_t pinfo)
{
	int choose 	= 0;
	while(1){
		atcom_print_cmd(pinfo);
		scanf("%d",&choose);
		getchar();
		if(choose < 0 || choose >= ARRAY_SIZE(cmd_name)){
			printf("\n choose wrong,please try again !\n");
			sleep(1);
		}
		else{
			break;
		}
	}

	return choose;
}

static int do_cmd_test(patcom_sub_lcd_t pinfo,int index)
{
	int ret = 0;
	int(*func)(patcom_sub_lcd_t pinfo);

	if( index >= ARRAY_SIZE(test_func) || index < 0 ){
		printf("Error: not support that function !\n");
		return -1;
	}
	
	func  = test_func[index];
	
	ret  = func(pinfo);
	if( ret < 0 ){
		printf("Error: run %s failed [ret=%d]\n",cmd_name[index]);
	}
	return ret;
}

static int atcom_sub_lcd_ioctl(patcom_sub_lcd_t pinfo)
{
	int 	ret = 0;
	int 	cmd_index = 0;
	while(1){
		cmd_index = atcom_choose_cmd(pinfo);
		if( cmd_index ==  ARRAY_SIZE(cmd_name) ){
			printf("You choose exit  cmd test \n");
			ret = 0;
			break;
		}else{
			ret = do_cmd_test(pinfo,cmd_index);	
			if( ret < 0 ){
				printf("Error: run %s failed !\n",cmd_name[cmd_index]);
				break;
			}
		}
	}
	return ret;
}



static int atcom_clear_screen(patcom_sub_lcd_t plcd)
{
	int ret 	= 0;
	int i 		= 0;
	int fd 		= 0;
	int fbsize 	= 0;
	unsigned char * pbuf= NULL;

	if( NULL == plcd ){
		printf("Error: param is NULL [%s:%d]!\n",__func__,__LINE__);
		return -1;
	}
	
	fd = plcd->fd;
	memset(plcd->pbuffer,0xFF,plcd->fbsize);
	
	//reflash lcd
	ret = ioctl(fd,UPDATE_LCD_SCREEN,NULL);
	if( ret < 0 )
	{
		printf("Error: ioctl UPDATE_LCD_SCREEN to reflash lcd failed [%s;%d]!\n",__func__,__LINE__);
		return -1;
	}	
	
	return 0;
}

static int atcom_reflash_lcd(patcom_sub_lcd_t plcd)
{
	int ret 	= 0;

	if( NULL == plcd ){
		printf("Error: param is NULL [%s:%d]!\n",__func__,__LINE__);
		return -1;
	}
	
	//reflash lcd
	ret = ioctl(plcd->fd,UPDATE_LCD_SCREEN,NULL);
	if( ret < 0 )
	{
		printf("Error: ioctl UPDATE_LCD_SCREEN to reflash lcd failed [%s;%d]!\n",__func__,__LINE__);
		return -1;
	}	

	return 0;
}



static int atcom_sub_lcd_draw_picture(patcom_sub_lcd_t pinfo)
{
	int ret	= 0;
	int row	= 0;
	int col	= 0;
	int index = 0;
	int col_max = 0;
	int row_max	= 0;
    struct fb_var_screeninfo  *pvinfo = NULL;
	pvinfo = &pinfo->vinfo;
	
	
	ret = atcom_clear_screen(pinfo);
	if( 0 !=  ret ){
		printf("Error: clear sublcd failed !\n");
		return ret;
	}	
	col_max  	= (pvinfo->xres*pvinfo->bits_per_pixel+7)/8;
	row_max		= pvinfo->yres;

	memset(pinfo->pbuf_bak,0xFF,pinfo->fbsize);

	for( row = 0 ; row < row_max; row++ ){
		for( col = 0; col < col_max ;col++){
			if( 0 == col % 8 ){
					pinfo->pbuf_bak[index] = ~0x03;
			}
			index++;
		} 	
	}	

	memcpy(pinfo->pbuffer,pinfo->pbuf_bak,pinfo->fbsize);
	
	ret = atcom_reflash_lcd(pinfo);
	if( 0 != ret ){
		printf("Error: reflash lcd picture failed !\n");
		return ret;
	}

	return ret;
}




int main(int argc ,char **argv)
{
	int	ret 	= 0;
	atcom_sub_lcd_t lcd_info;
	char choose_ioctl 	= 0;
	
	memset(&lcd_info,0x00,sizeof(lcd_info));	
	
	ret = atcom_lcd_init(&lcd_info);
	if( ret < 0 ){
		printf("Error: atcomc init lcd information failed [%s:%d]!\n",__func__,__LINE__);
		return -1;
	}	
	printf("do you wanna draw a string on lcd: yes[Y|y] no[N|n] !\n");
	scanf("%c",&choose_ioctl);
	getchar();
	if( 'Y' == choose_ioctl || 'y' == choose_ioctl ){
		ret = atcom_sub_lcd_draw_picture(&lcd_info);
		if( ret < 0 ){
			ret = -1;
			goto error;
		}
	}

	printf("do you wanna test sub lcd ioctls function: yes[Y|y] no[N|n]\n");
	scanf("%c",&choose_ioctl);
	getchar();
	if( 'Y' == choose_ioctl || 'y' == choose_ioctl ){
		ret = atcom_sub_lcd_ioctl(&lcd_info);
		if( ret < 0 ){
			ret = -1;
			goto error;
		}
	}





	atcom_lcd_uninit(&lcd_info);
	return  0;

error:
	atcom_lcd_uninit(&lcd_info);
	return ret;
}

















