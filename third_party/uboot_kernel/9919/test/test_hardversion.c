#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#define  VERSION_DEV_NAME "/dev/atcom_version"

#define VERSION_LEN         128

#ifdef OLD_VERSION
typedef struct Kernel_VERION_{
        unsigned char kernel_version[VERSION_LEN];
        unsigned char kernel_describe[VERSION_LEN];
        unsigned char uboot_version[VERSION_LEN];
        unsigned char uboot_describe[VERSION_LEN];
}kernel_version_t,*pkernel_version_t;
#else
 typedef struct Kernel_VERION_{
     unsigned char kernel_version[VERSION_LEN];
     unsigned char kernel_describe[VERSION_LEN];
     unsigned char uboot_version[VERSION_LEN];
     unsigned char uboot_describe[VERSION_LEN];
     int           hardware_version;
     unsigned char hardware_describe[VERSION_LEN];
}kernel_version_t,*pkernel_version_t;
#endif


#define KERNEL_IOCTL_BASE           'K'
#define GET_KERNEL_VERDION          _IOR(KERNEL_IOCTL_BASE, 0x01, kernel_version_t)

#ifdef OLD_VERSION
void printf_version_info(kernel_version_t* pinfo)
{
    printf("\n kernel_version:%s\n",pinfo->kernel_version);
    printf("\n kernel_describe:%s\n",pinfo->kernel_describe);
    printf("\n uboot_version:%s\n",pinfo->uboot_version);
    printf("\n uboot_describe:%s\n",pinfo->uboot_describe);
    return ;
}
#else
void printf_version_info(kernel_version_t* pinfo)
{
    printf("\n kernel_version:%s\n",pinfo->kernel_version);
    printf("\n kernel_describe:%s\n",pinfo->kernel_describe);
    printf("\n uboot_version:%s\n",pinfo->uboot_version);
    printf("\n uboot_describe:%s\n",pinfo->uboot_describe);

    printf("\n hardware versiob :0x%x\n",pinfo->hardware_version);
    printf("\n hardware_describe:%s\n",pinfo->hardware_describe);
    return ;
}
#endif


int main(int argc,char ** argv)
{
    int ret = 0;
    kernel_version_t version_info;
    int fd = 0;
    fd = open(VERSION_DEV_NAME,O_RDWR);
    if( fd < 0 )
    {
        printf("open %s failed !\n",VERSION_DEV_NAME);
        return -1;
    }

    memset(&version_info,0x00,sizeof(version_info));
    ret =  ioctl(fd,GET_KERNEL_VERDION,&version_info);
    if( ret < 0 )
    {
        printf("ioctl for get verion info failed !\n");
        return -1;
    }


    printf_version_info(&version_info);


    return ret;

}
