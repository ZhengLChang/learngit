typedef enum{
	PIN_DOWN=0,
	PIN_UP,
}shutdown_pin_status;
#define ATCOM_EXT_RESET_IOW(num, dtype)         _IOW('u', num, dtype)
#define ATCOM_EXT_RESET_IOR(num, dtype)         _IOR('u', num, dtype)
#define ATCOM_EXT_RESET_IOWR(num, dtype)        _IOWR('u', num, dtype)
#define ATCOM_EXT_RESET_IO(num)                 _IO('u', num)
#define ATCOM_EXT_RESET_GET_STATUS            	 ATCOM_EXT_RESET_IOR(32,shutdown_pin_status)
#define ATCOM_EXT_RESET_SET_STATUS			     ATCOM_EXT_RESET_IOW(33,shutdown_pin_status)


