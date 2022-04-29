#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <stdlib.h>

int get_configuration(libusb_device *dev,struct libusb_config_descriptor *config)
{
	int nRet = 0;
	nRet = libusb_get_config_descriptor(dev,0,&config);
	return nRet;
}

static void dump_altsetting(libusb_device_handle *dev,const struct libusb_interface_descriptor *interface)
{
	printf("	Interface Descriptor:\n"
	       "	bLength	-- 描述长度		%5u\n"
	       "	bDescriptorType	-- 描述符类型	%5u\n"
	       "	bInterfaceNumber -- 接口编号	%5u\n"
	       "	bAlternateSetting -- 备用编号	%5u\n"
	       "	bNumEndpoints -- 所使用的端点号		%5u\n"
	       "	bInterfaceClass -- 所使用的类		%5u\n"
	       "	bInterfaceSubClass -- 所使用的子类	%5u\n"
	       "	bInterfaceProtocol -- 所使用的协议	%5u\n",
	       interface->bLength,interface->bDescriptorType,interface->bInterfaceNumber,interface->bAlternateSetting,interface->bNumEndpoints,interface->bInterfaceClass,
	       interface->bInterfaceSubClass,interface->bInterfaceProtocol);

}

static void dump_interface(libusb_device_handle *dev,const struct libusb_interface *interface)
{
	int i;
	for(i=0;i<interface->num_altsetting;i++)
		dump_altsetting(dev,&interface->altsetting[i]);
}

static int list_devices(libusb_context *ctx)
{
	libusb_device **list;
	struct libusb_device_descriptor desc;
	struct libusb_config_descriptor *conf;
	libusb_device_handle * handle = NULL;
	int config = 0;
	int ret;
	int status;
	ssize_t num_devs,i,j,k;
	status = 1;/*1 device is not found,0 device is found*/
	num_devs = libusb_get_device_list(ctx,&list);/*获取当前usb设备列表*/
	if(num_devs < 0)
		goto error;
	for(i=0;i<num_devs;i++)
	{
		libusb_device *dev = list[i];
		libusb_open(dev,&handle);/*打开一个设备并获取句柄*/
		//libusb_get_configuration(handle,&config);/*确定当前活动配置的bConfigurationValue*/
		printf("55\n");
		uint8_t bnum = libusb_get_bus_number(dev);/*获取设备连接的总线编号*/
		uint8_t dnum = libusb_get_device_address(dev);/*获取连接到总线设备的地址*/
		libusb_get_device_descriptor(dev,&desc);/*获取给定设备的设备描述符*/
		status = 0;
		printf("deivce:%04x:%04x\n",desc.idVendor,desc.idProduct);
		printf("bDeviceSubClass = %5u\n",desc.bDeviceSubClass);
		printf("bDeviceClass 	= %5u\n",desc.bDeviceClass);
		printf("bDeviceProtocol = %5u\n",desc.bDeviceProtocol);
		for(j=0;j<desc.bNumConfigurations;++j)/*根据USB设备的配置数*/
		{
			ret = libusb_get_config_descriptor(dev,j,&conf);/*根据索引获取USB配置描述符*/
			if(ret)
			{
				fprintf(stderr,"Couldn't get configuration"
						"descriptor %1u,some information will"
						"be missing\n",j);
			}
			else
			{
				printf("bNumberInterfaces = %5u\n",conf->bNumInterfaces);
				printf("bConfigurationValue = %5u\n",conf->bConfigurationValue);
				for(k=0;k<conf->bNumInterfaces;k++)
				{
					dump_interface(handle,&conf->interface[k]);
				
				}
				libusb_free_config_descriptor(conf);
			}
		}

	}
	printf("86\n");
	libusb_free_device_list(list,0);
error:
	return status;
}

int main(int argc,char *argv[])
{
	int err = 0;
	libusb_context *ctx;
	err = libusb_init(&ctx);/*libusb 初始化*/
	
	if(err)
	{
		fprintf(stderr,"unable to initialize libusb : %i\n",err);
		return EXIT_FAILURE;
	}
	list_devices(ctx);
	
	return 0;
}

