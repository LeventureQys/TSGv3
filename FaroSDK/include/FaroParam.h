enum class FaroStatus
{
	DEFAULT = 0,	//起始的状态
	INITFAROING,	//正在初始化扫描仪
	INITFARO,		//扫描仪初始化状态
	//PINGING,		//ping中
	//PING,			//ping完
	//INITSDKING,	//正在初始化SDK
	//INITSDK,		//SDK初始化的状态
	CONNECTING,		//正在连接状态
	CONNECT,		//已连接状态
	TIMEOUT,		//法如扫描仪链接超时
	SETDEVICE,			//法如扫描仪设置
	SETMISSION,			//设置任务信息

	RUNCAMING,		//正在启动镜头
	RUNCAM,			//启动镜头状态
	RECORDFAROING,	//正在开始记录数据状态
	RECORDFARO,		//记录数据状态
	PAUSEFARPING,	//正在暂停
	PAUSEFARO,		//暂停状态
	STOPFAROING,	//正在结束扫描
	//STOPFARO		//结束扫描状态
};