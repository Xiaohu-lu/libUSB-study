#include <stdio.h>
#include <libusb.h>
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
	       "	bLength			%5u\n"
	       "	bDescriptorType		%5u\n"
	       "	bInterfaceNumber	%5u\n"
	       "	bAlternateSetting	%5u\n"
	       "	bNumEndpoints		%5u\n"
	       "	bInterfaceClass		%5u\n"
	       "	bInterfaceSubClass	%5u\n"
	       "	bInterfaceProtpcpl	%5u\n",
	       interface->bLength,interface->bDescriptor,interface->bInterfaceNumber,interface->bAlternateSetting,interface->bNumEndpoints,interface->bInterfaceClass,
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
	libusb_deivce_handle * handle = NULL;
	int config = 0;
	int ret;
	int status;
	ssize_t num_devs,i,j,k;
	status = 1;/*1 device is not found,0 device is found*/
	num_devs = libusb_get_device_list(ctx,&list);
	if(num_devs < 0)
		goto error;
	for(i=0;i<num_devs;i++)
	{
		libusb_device *dev = list[i];
		libusb_open(dev,&handle);
		libusb_get_configuration(handle,&config);
		uint8_t bnum = libusb_get_bus_number(dev);
		uint8_t dnum = libusb_get_device_address(dev);
		libusb_get_device_descriptor(dev,&desc);
		status = 0;
		printf("deivce:%04x:%04x\n",desc.idVendor,desc.idProduct);
		printf("bDeviceSubClass = %5u\n",desc.bDeviceSubClass);
		printf("bDeviceClass 	= %5u\n",desc.bDeviceClass);
		printf("bDeviceProtocol = %5u\n",desc.bDeviceProtocol);
		for(j=0;j<desc.bNumConfigurations;++j)
		{
			ret = libusb_get_config_descriptor(dev,j,&conf);
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
	libusb_free_device_list(list,0);
error:
	return status;
}

int main(int argc,char *argv[])
{
	int err = 0;
	libusb_context *ctx;
	err = libusb_init(&ctx);
	if(err)
	{
		fprintf(stderr,"unable to initialize libusb : %i\n",err);
		return EXIT_FAILURE;
	}
	list_devices(ctx);
	return 0;
}
