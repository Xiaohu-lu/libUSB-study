/*USBlib的一些变量类型重定义*/
typedef volatile LONG usbi_atomic_t;

/*USBlib的一些结构体*/
1.libusb_device 在file->libusbi.h中（libusb-1.0.26-\ibusb）
struct libusb_device{
	usbi_atomic_t	refcnt;
	struct libusb_context *ctx;
	struct libusb_device *parent_dev;
	
	uint8_t bus_number;						/*设备连接的总线编号*/
	uint8_t port_number;
	uint8_t device_address;					/*连接到总线上的设备的地址*/
	enum libusb_speed speed;
	
	struct list_head list;
	unsigned long session_data;
	
	struct libusb_device_descriptor device_descriptor;
	usbi_atomic_t 	attached;
};

2.libusb_context 在file->libusbi.h中
struct libusb_context{
#if define(ENABLE_LOGGING) && !define(ENABLE_DEBUG_LOGGING)
	enum libusb_log_level debug;
	int debug_fixed;
	libusb_log_cb log_handler;
#endif
	/*用于发生内部时间的信号*/
	usbi_event_t	event;
#ifdef	HAVE_OS_TIMER
	/*如果操作系统支持，用于超时处理。维护此计时器以在下一个未绝超时触发*/
	usbi_timer_t timer;
#endif
	struct list_head usb_devs;
	usbi_mutex_t usb_devs_lock;
	/*已注册的热插拔回调列表
	*/	
	struct list_head hotplug_cbs;
	libusb_hotplug_callback_handle next_hotplug_cb_handle;
	usbi_mutex_t hotplug_cbs_lock;
	
	/*指示上下文已经准备好接受热插拔通知的标志*/
	usbi_atomic_t hotplug_ready;
	
	/*这是一个动态传输句柄列表，按超时到期排序。最快超时的URB放在列表的开头，稍后超时的URB放在后面，无限超时的URB总三放在最后面*/
	struct	list_head flying_transfers;
	/*注意同时使用this和usbi_transfer->lock的路径必须现使用次锁*/ 
	usbi_mutex_t flying_transfers_lock;
#if !defined(PLATFORM_WINDOWS)
	/*pollfd 的变化，用户回调函数*/
	libusb_pollfd_added_cb fd_added_cb;
	libusb_pollfd_removed_cb fd_removed_cb;
	void *fd_cb_user_data;
#endif
	/*确保任何时候只有一个线程在处理事件*/
	usbi_mutex_t events_lock;
	
	/*用于查看是否有活动线程在进行事件处理*/
	int event_handler_active;
	
	/*用于跟踪那个线程正在执行事件处理的线程本地存储*/
	usbi_tls_key_t event_handling_key;
	
	/*用于在事件处理线程以外的线程中等待事件完成*/
	usbi_mutex_t event_waiters_lock;
	usbi_cond_t event_waiters_cond;
	
	/*保护内部上下文事件数据的锁*/
	usbi_mutex_t event_data_lock;
	
	/*设置为指示需要处理的特定事件的标志位掩码。受event_data_lock保护*/
	unsigned int event_flags;
	
	/*当我们想要中断和阻止事件处理时设置的计数器，以便安全地关闭设备。受event_data_lock保护*/
	unsigned int device_close;
	
	/*当前活动的事件源列表.受事件数据锁保护*/
	struct list_head event_sources;
	
	/*自上次等待事件源以来已删除的事件源列表。受event_data_lock保护*/
	struct list_head removed_event_sources;
	
	/*指向用于监视事件源的特定于平台的数据的指针和计数。仅在事件处理期间访问*/
	void *event_data;
	unsigned int event_data_cnt;
	
	/*待处理的热插拔消息列表，受event_data_lock保护*/
	struct list_head hotplug_msgs;
	
	/*待完成传输的列表，受event_data_lock保护*/
	struct list_head completed_transfers;
	
	struct list_head list;
};
	
3.list_head	
struct list_head{
	struct list_head *pre,*next;
};
	
4.libusb_config_descriptor 	usb配置描述符结构体
struct libusb_config_descriptor{
	uint8_t bLength;							/*bLength, 该描述符的长度*/
	uint8_t bDescriptorType;					/*bDescriptor,	描述符类型*/
	uint16_t wTotalLength;						/*此配置返回的总数据长度*/
	uint8_t bNumInterfaces;						/*此配置支持的接口数*/
	uint8_t bConfigurationValue;				/*此配置的标识符*/
	uint8_t	iConfiguration;						/*描述该配置的字符串的索引值*/
	uint8_t bmAttributes;						/*该设备的属性，总线供电，不支持远程唤醒*/
	uint8_t MaxPower;							/*设备所需要的电流,200mA最大电流*/
	const struct libusb_interface *interface;	/*此配置支持的接口数组,此数组的长度由bNumInterface决定*/
	const unsigned char *extra;					/*额外的描述符,如果libusb遇到未知的配置描述符,会将它存到这里,如果你希望解析它们*/
	int extra_length;							/*额外描述符的长度,以字节为单位,必须为非负数*/
};

5.libusb_device_handle 	 usb设备句柄
struct libusb_device_handle{
	/*锁保护*/
	usbi_mutex_t lock;
	unsigned long claimed_interfaces;
	struct list_head list;
	struct libusb_device *dev;
	int auto_detach_kernel_driver;
};

6.libusb_interface_descriptor usb接口描述符结构体
struct libusb_interface_descriptor{
	uint8_t bLength;							/*bLength, 该描述符的长度*/		
	uint8_t bDescriptorType;					/*bDescriptor,	描述符类型*/
	uint8_t bInterfaceNumber;					/*bInterfaceNumber,	该接口的编号（从0开始）*/
	uint8_t bAlternateSetting;					/*bAlternateSetting, 该接口的备用编号*/
	uint8_t bNumEndpoints;						/*bNumEndpoints,	接口所使用的端点数*/
	uint8_t bInterfaceClass;					/*bInterfaceClass,	该接口所使用的类*/
	uint8_t bInterfaceSubClass;					/*bInterfaceSubClass, 该接口所使用的子类*/
	uint8_t bInterfaceProtocol;					/*bInterfaceProtocol,	该接口所使用的协议*/
	uint8_t iInterface;							/*iInterface,	描述该接口的字符串的索引值*/
	const struct libusb_endpoint_descriptor *endpoint;	/*端点描述符数组,数据长度由bNumEndpoints决定*/
	const unsigned char *extra;					/*额外的描述符,如果libusb遇到未知的配置描述符,会将它存到这里,如果你希望解析它们*/
	int extra_length;							/*额外描述符的长度,以字节为单位,必须为非负数*/
};
	
7.libusb_endpoint_descriptor usb端点描述符结构体	
struct libusb_endpoint_descriptor{
	uint8_t bLength;							/*bLength, 该描述符的长度*/		
	uint8_t bDescriptorType;					/*bDescriptor,	描述符类型*/
	uint8_t bEndpointAddress;					/*bEndpointAddress, 端点的地址,0x81，D7=1，表示传输方向输入，表示设备传输数据到主机*/
	uint8_t bmAttributes;						/*bmAttributes,	端点的属性,D1~D0表示端点的传输类型,2批量传输*/
	uint16_t wMaxPacketSize;					/*wMaxPackeSize,	该端点支持的最大包长度,0x00 0x40,64字节*/
	uint8_t bInterval;							/*bInterval,	端点的查询时间*/
	uint8_t bRefresh;							/*bRefresh,	只针对音频设备,提供同步反馈的速率*/
	uint8_t bSynchAddress;						/*bSynchAddress,	只针对音频设备,同步端点的地址*/
	const unsigned char *extra;					/*额外的描述符,如果libusb遇到未知的配置描述符,会将它存到这里,如果你希望解析它们*/
	int extra_length;							/*额外描述符的长度,以字节为单位,必须为非负数*/
};

8.libusb_device_descriptor usb的设备描述符结构体
struct libusb_device_descriptor{
	uint8_t bLength;							/*bLength, 该描述符的长度*/
	uint8_t bDescriptorType;					/*bDescriptor,	描述符类型*/
	uint16_t bcdUSB;							/*bcdUSB, 本设备所使用的USB协议规范 0x0110-USB1.1*/
	uint8_t bDeviceClass;						/*bDevicClass,	USB分配的设备类代码，0x01~0xfe为标准设备类,0xff为厂商自定义类型，0x00不是在设备描述符中定义的，如HID*/
	uint8_t bDeviceSubClass;					/*bDeviceSubClass,	USB分配的设备子类代码，类代码为0，子类代码也必须为0*/
	uint8_t bDeviceProtocol;					/*bDeviceProtocl,	USB分配的设备协议代码,类代码为0，协议代码也必须为0*/
	uint8_t bMaxPacketSize0;					/*bMaxPackSize0,	端点0最大包长,64*/
	uint16_t idVendor;							/*idVender,		厂商ID=0x2109*/		
	uint16_t idProduct;							/*idProduct,	产品ID=0x7638*/
	uint16_t bcdDevice;							/*bcdDevice,	设备版本号*/
	uint8_t iManufacturer;						/*iManufacturer,描述厂商的字符串的索引，Vender的索引*/
	uint8_t iProduct;							/*iProduct,	描述产品的字符的索引 Product的索引*/
	uint8_t iSerialNumber;						/*iSerialNumber,	产品序列号字符串的索引,SN的索引*/
	uint8_t bNumConfigurations;					/*bNumConfigurations,	可能的配置数，大部分USB设备只有一种配置*/
};

/*USBlib的一些函数*/	
1.libusb_init(libusb_context **ctx); 初始化libusb,该函数必须先调用在调用其他任何函数之前
2.libusb_get_device_list(libusb_context *ctx,libusb_device ***list);返回连接到系统的USB设备列表,这是寻找要操作的USB设备的入口点;返回值是当前USB设备个数
3.libusb_open(libsub_device *dev,libusb_device_handle **dev_handle);打开一个设备并获取一个设备句柄,句柄允许你在设备上执行I\O操作
4.libusb_get_configuration(libusb_device_handle *dev_handle,int *config);确定当前活动配置的bConfigurationValue
5.libusb_get_bus_number(libusb_device *dev);获取设备连接的总线编号
6.libusb_get_device_address(libusb_device *dev);获取连接到总线设备的地址
7.libusb_get_device_descriptor(libusb_device *dev,struct libusb_device_descriptor *desc);获取给定设备的设备描述符
8.libusb_get_config_descriptor(libusb_device *dev,uint8_t config_index,struct libusb_config_descriptor **config);根据索引获取USB配置描述符
9.libusb_free_config_descriptor(struct libsub_config_descriptor *config);释放一个获取到的配置描述符
	
	
	
