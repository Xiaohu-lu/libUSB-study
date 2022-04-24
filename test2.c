#include <libusb.h>
#include <stdio.h>


int main(int argc,char *argv[])
{
	/*初始化上下文*/
	int nRet = libusb_init(NULL);
	if(nRet < 0)
	{
		printf("libusb_init(NULL) failed:[%s] \n",libusb_strerror(nRet));
		return -1;
	}
	printf("libusb_init(NULL) ok \n");
	/**/
}
