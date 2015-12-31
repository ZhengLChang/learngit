#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>


//14byte�ļ�ͷ
typedef struct
{
	char cfType[2];//�ļ����ͣ�"BM"(0x4D42)         
	long cfSize;//�ļ���С���ֽڣ�         
	long cfReserved;//������ֵΪ0     
	long cfoffBits;//������������ļ�ͷ��ƫ�������ֽڣ�    
}__attribute__((packed)) BITMAPFILEHEADER;
//__attribute__((packed))�������Ǹ��߱�����ȡ���ṹ�ڱ�������е��Ż����룬����ʵ��ռ���ֽ������ж���
//40byte��Ϣͷ
typedef struct
{
	char ciSize[4];//BITMAPFILEHEADER��ռ���ֽ���
	long ciWidth;//       
	long ciHeight;//       
	char ciPlanes[2];//Ŀ���豸��λƽ������ֵΪ1
	int ciBitCount;//ÿ�����ص�λ��
	char ciCompress[4];//ѹ��˵��
	char ciSizeImage[4];//���ֽڱ�ʾ��ͼ���С�������ݱ�����4�ı���    
	char ciXPelsPerMeter[4];//Ŀ���豸��ˮƽ������/��
	char ciYPelsPerMeter[4];//Ŀ���豸�Ĵ�ֱ������/��
	char ciClrUsed[4]; //λͼʹ�õ�ɫ�����ɫ��   
	char ciClrImportant[4]; //ָ����Ҫ����ɫ�����������ֵ������ɫ��ʱ�����ߵ���0ʱ������ʾ������ɫ��һ����Ҫ
}__attribute__((packed)) BITMAPINFOHEADER;
typedef struct
{
	unsigned short red:5;
	unsigned short green:6;
	unsigned short blue:5;
}__attribute__((packed)) PIXEL;//��ɫģʽ��RGB565

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





	//��ת��������
	fseek(fp, FileHead.cfoffBits, SEEK_SET);
	//ÿ���ֽ���
	BytesPerLine = (InfoHead.ciWidth * InfoHead.ciBitCount + 31) / 32 * 4;   

	line_x = line_y = 0;

	//��framebuffer��дBMPͼƬ
	while(!feof(fp)){
		PIXEL pix;
		unsigned short int tmp;
		rc = fread( (char *)&pix, 1, sizeof(unsigned short int), fp);
		if (rc != sizeof(unsigned short int)){ 
			break; 
		}
		location = line_x * bits_per_pixel / 8 + (InfoHead.ciHeight - line_y - 1) * xres * bits_per_pixel / 8;

		//��ʾÿһ������
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

	//����ʾ�豸
	fbfd = open("/dev/fb0", O_RDWR);
	if (!fbfd)
	{
		printf("Error: cannot open framebuffer device.\n");
		exit(1);
	}

	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
	{
		printf("Error��reading fixed information.\n");
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

	//������Ļ���ܴ�С���ֽڣ�
	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	printf("screensize=%d\n",screensize);

	//�ڴ�ӳ��
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

