/*USBlib的一些变量类型重定义*/
typedef volatile LONG usbi_atomic_t;

/****USBlib的一些结构体***/
1.libusb_device 在file->libusbi.h中（libusb-1.0.26-\ibusb）
struct libusb_device{
	usbi_atomic_t	refcnt;
	struct libusb_context *ctx;
	struct libusb_device *parent_dev;
	
	uint8_t bus_number;
	uint8_t port_number;
	uint8_t device_address;
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
	
	
	
	
	
	
