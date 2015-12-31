#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>


//14byte文件头
typedef struct
{
	char cfType[2];//文件类型，"BM"(0x4D42)         
	long cfSize;//文件大小（字节）         
	long cfReserved;//保留，值为0     
	long cfoffBits;//数据区相对于文件头的偏移量（字节）    
}__attribute__((packed)) BITMAPFILEHEADER;
//__attribute__((packed))的作用是告诉编译器取消结构在编译过程中的优化对齐，按照实际占用字节数进行对齐
//40byte信息头
typedef struct
{
	char ciSize[4];//BITMAPFILEHEADER所占的字节数
	long ciWidth;//       
	long ciHeight;//       
	char ciPlanes[2];//目标设备的位平面数，值为1
	int ciBitCount;//每个像素的位数
	char ciCompress[4];//压缩说明
	char ciSizeImage[4];//用字节表示的图像大小，该数据必须是4的倍数    
	char ciXPelsPerMeter[4];//目标设备的水平像素数/米
	char ciYPelsPerMeter[4];//目标设备的垂直像素数/米
	char ciClrUsed[4]; //位图使用调色板的颜色数   
	char ciClrImportant[4]; //指定重要的颜色数，当该域的值等于颜色数时（或者等于0时），表示所有颜色都一样重要
}__attribute__((packed)) BITMAPINFOHEADER;
typedef struct
{
	unsigned short red:5;
	unsigned short green:6;
	unsigned short blue:5;
}__attribute__((packed)) PIXEL;//颜色模式，RGB565

BITMAPFILEHEADER FileHead;
BITMAPINFOHEADER InfoHead;

static char *fbp = 0;
static int xres = 0;
static int yres = 0;
static int bits_per_pixel = 0;


int show_bmp()
{
	FILE *fp;
	int rc;
	int line_x, line_y;
	long int location = 0, BytesPerLine = 0;
	char tmp[1024*10];

	fp = fopen( "R3.bmp", "rb" );
	if (fp == NULL)
	{
		printf("open R3.bmp error\n");
		return( -1 );
	}

	rc = fread( &FileHead, sizeof(BITMAPFILEHEADER),1, fp );
	if ( rc != 1)
	{
		printf("read header error!\n");
		fclose( fp );
		return( -2 );
	}

	if (memcmp(FileHead.cfType, "BM", 2) != 0)
	{
		printf("it's not a BMP file\n");
		fclose( fp );
		return( -3 );
	}

	rc = fread( (char *)&InfoHead, sizeof(BITMAPINFOHEADER),1, fp );
	if ( rc != 1)
	{
		printf("read infoheader error!\n");
		fclose( fp );
		return( -4 );
	}





	//跳转的数据区
	fseek(fp, FileHead.cfoffBits, SEEK_SET);
	//每行字节数
	BytesPerLine = (InfoHead.ciWidth * InfoHead.ciBitCount + 31) / 32 * 4;   

	line_x = line_y = 0;

	//向framebuffer中写BMP图片
	while(!feof(fp)){
		PIXEL pix;
		unsigned short int tmp;
		rc = fread( (char *)&pix, 1, sizeof(unsigned short int), fp);
		if (rc != sizeof(unsigned short int)){ 
			break; 
		}
		location = line_x * bits_per_pixel / 8 + (InfoHead.ciHeight - line_y - 1) * xres * bits_per_pixel / 8;

		//显示每一个像素
		tmp=pix.red<<0 | pix.green<<5 | pix.blue<<11;
		*((unsigned short int*)(fbp + location)) = tmp;

		line_x++;
		if (line_x == InfoHead.ciWidth ){
			line_x = 0;
			line_y++;

			if(line_y == InfoHead.ciHeight){
				break;
			}
		}
	}

	fclose( fp );

	return( 0 );
}


typedef struct RGB24{
	char b;
	char g;
	char r;
}RGB24_t,*pRGB24_t;

typedef struct RGB16{
	unsigned short data;
}RGB16_t,*pRGB16_t;



static void show_3_tritile()
{
	long int screensize = 0;
	int row  = 0;
	int col	 = 0;
	char *pbuf = NULL; 
	RGB24_t red_24={
		.r = 0XFF,
		.g = 0x00,
		.b = 0x00,
	}; 
	RGB24_t greed_24={
		.r = 0X00,
		.g = 0xFF,
		.b = 0x00,
	}; 
	RGB24_t blue_24={
		.r = 0X00,
		.g = 0x00,
		.b = 0xFF,
	}; 

	RGB16_t red_16={
		.data = 0xf100,
	}; 
	RGB16_t greed_16={
		.data = 0x0e70,
	}; 
	RGB16_t blue_16={
		.data = 0x008f,
	}; 

	RGB16_t * p16 = NULL;
	RGB24_t * p24 = NULL;

	screensize = (xres * yres * bits_per_pixel)/8;
	pbuf = (char * )malloc(screensize);
	if( NULL == pbuf ){
		printf("malloc failed !\n");
		return;
	}

	printf("xres=%d, yres=%d, bits_per_pixel=%d\n",xres,yres,bits_per_pixel);

	while(1){
		if( 16 == bits_per_pixel ){
			p16		= (RGB16_t *)pbuf;
			for( row = 0 ; row < yres ; row++){
				for( col = 0 ; col < xres ; col++){
					if( row < yres/3){ //red
						*p16++ = red_16;
					}else if( row >= yres/3 && row <= yres*2/3){  //greed
						*p16++ = greed_16;
					}else{//blue
						*p16++ = blue_16;
					}
				}		
			}
			
		}else{
			p24	= (RGB24_t *)pbuf;
			for( row = 0 ; row < yres ; row++){
				for( col = 0 ; col < xres ; col++){//red
					if( row < yres/3){
						*p24++ = red_24;
					}else if( row >= yres/3 && row <= yres*2/3){//greed
						*p24++ = greed_24;
					}else{//blue
						*p24++ = blue_24;			
					}
				}		
			}		
		}
		memcpy(fbp,pbuf,screensize);
	}
	return ;	
}




int main ( int argc, char *argv[] )
{
	int fbfd = 0;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	long int screensize = 0;

	//打开显示设备
	fbfd = open("/dev/fb0", O_RDWR);
	if (!fbfd)
	{
		printf("Error: cannot open framebuffer device.\n");
		exit(1);
	}

	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
	{
		printf("Error：reading fixed information.\n");
		exit(2);
	}

	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
	{
		printf("Error: reading variable information.\n");
		exit(3);
	}

	printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );
	xres = vinfo.xres;
	yres = vinfo.yres;
	bits_per_pixel = vinfo.bits_per_pixel;

	//计算屏幕的总大小（字节）
	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	printf("screensize=%d\n",screensize);

	//内存映射
	fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if ((int)fbp == -1)
	{
		printf("Error: failed to map framebuffer device to memory.\n");
		exit(4);
	}
	printf("sizeof header=%d\n", sizeof(BITMAPFILEHEADER));
	show_3_tritile();

//	printf("into show_bmp function\n");
//	show_bmp();
	getchar();
	munmap(fbp, screensize);
	close(fbfd);
	return 0;
}

