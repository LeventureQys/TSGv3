#include "TSG_Framework.h"
#include "SerialParams.h"
#include "qserialport.h"
#include "qqueue.h"
#include "PublicAlgorithm.h"
#include "qthread.h"
#include "qmutex.h"
#include "qserialportinfo.h"
#include "qdatastream.h"
//串口初始化参数
enum class MainServiceType {
	MS101,			//小车，MS101
	ZS_CRTG,		//中隧CRTG
	ZS_Whole,		//全幅设备
	ZS_Half,		//半幅设备
	HS_VTM,			//车载设备
	MS201			//三叶草码盘
};

class SerialPortInitParam {
public:
	void SetSerialPortType(MainServiceType type) {
		SP_Type = type;
		//暂时不做处理
	}
	MainServiceType SP_Type = MainServiceType::MS101;
	double dbCarDeta = 0.000084368943;				//小车设置比率
	qint32 nTimeFactory = 10;		//时间倍数，不同的设备的时间是不同的，默认时间基准为1us,也就是所有	输出的时间*时间系数 的单位应该是1us
	QSerialPortInfo serialInfo;
};



class serialPortState {
public:
	bool blnGrapping = false;
	bool blnTrolleyRunning = false;
	bool blnLight = false;
	bool blnCameraEnable = false;
	qint32 nExposure = 0;
	qint32 nCloverSpeed = 0;
	QList<qint32> list_position;
	qint32 nScannerTimeID = 0;
	qint64 nCurrentTimeDiff = 0;
	bool blnDirection = false;
	qint32 nTemperature = 0;
	qint16 nTrolleySpeed = 0;
	quint8 nBatteryUsage = 0;
	quint8 nKnobState = 0;
	quint8 nEncoder = 0;
	quint8 nKeyMode = 0;
	bool blnScannerPower = false;
	quint16 nMinSpeed = 0;
	quint16 nMaxSpeed = 0;
	qint32 lnInternalTime = -1;				//板载时间
	qint32 nMileageID = 0;
	SerialPortInitParam initParam;
};

/// <summary>
/// 串口控制对象，专门用于控制串口的设备对象
/// </summary>
/// 
using namespace SPP;
//注册宏
#define REG(byte,methodName)\
	this->map_ret.insert(byte,[this](const QByteArray& arr) {this->methodName ## RET(arr); });
class serialPort :public TSG_Framework {
	Q_OBJECT

public:
	/// <summary>
/// 串口类型
/// </summary>
	enum class SerialPortType {
		Normal,	//普通串口
		Imu,	//IMU串口
		Picture	//图像串口
	};

	serialPort(QObject* obj);
	~serialPort();
	/// <summary>
	/// 初始化串口参数
	/// </summary>
	/// <param name="init">初始化参数组</param>
	/// <returns>初始化是否成功</returns>
	bool InitSerialPort(const SerialPortInitParam& init);

	bool SetMission(const MissionContent& mission);
	bool PrewarmMachine();	//机器预热
	bool StartMission();	//开始任务
	bool PauseMission();	//暂停任务
	bool EndMission();		//结束任务
	bool AnalyseResult();	//分析结果 
Q_SIGNALS:
	/// <summary>
	/// 串口绑定成功的信号
	/// </summary>
	void Sig_SerialPortInitSuccess(const SerialPortInitParam& initParams, const SerialPortType& type);
	/// <summary>
	/// 接收到串口消息的信号
	/// </summary>
	/// <param name="code">任务码</param>
	/// <param name="message">消息体</param>
	void Sig_SerialPortMessage(const QByteArray& code, const QVariant& message);

	/// <summary>
	/// 从下层上传消息
	/// </summary>
	void Sig_UpdateMessage(const serialPortState& message, const QByteArray& code, const QList<QVariant>& obj);
private:
	const QString strTab = "\t";
	const QString str_classname = "SerialPort";
	SerialPortInitParam InitParams;
	MissionContent mission;
	SerialPortStatus status = SerialPortStatus::WS_None;
	/// <summary>
	/// 控制串口的指针
	/// </summary>
	QSerialPort* m_serialPort = nullptr;
	bool blnInitSuccess = false;
	/// <summary>
	/// 串口消息的缓冲区，因为可能存在截断的情况，所以需要缓冲区
	/// </summary>
	QByteArray buffer;
	QMap<QByteArray, qint32> map_LengthSp;
	/// <summary>
	/// 发送测试命令
	/// </summary>
	serialPortState state;

	void UpdateMessage(const QByteArray& code, const QList<QVariant>& obj);
	//接到ReadyRead后的处理函数
	void CheckPortContains();
	//处理buffer内的数据
	void ProcessBuffer();
	SerialMessage splitMessage(const QByteArray& arr);
	/// <summary>
	/// 初始化指令长度对象，需要在串口内部保留这部分功能，因为不同的设备的指令长度不一样
	/// </summary>
	void Init_map_LengthSp();
	void InitDefaultParamsByDeviceName();
	qint8 GetFunctionLength(const QByteArray& type);	//根据消息码找到长度 
	/// <summary>
	/// 解析buffer内的数据
	/// </summary>
	/// <returns></returns>
	QQueue<QByteArray> parseByteArray();
	/// <summary>
	/// 存储消息列表
	/// </summary>
	QQueue<QByteArray> queue_byte;
	QByteArray CRC16_2(const QByteArray& arr);
	/// <summary>
	/// 将任务注册到map中去
	/// </summary>
	void RegistFunctions();
	bool writeData(const QByteArray& data);

	QMap<QByteArray, std::function<void(QByteArray)>> map_ret;		//返回结果的映射表

	void registFunctions();

#pragma region public
	double Pai = 3.14159265;
	QByteArray PositionToHexadecimal(const QList<qint32>& cameras);
	QByteArray int32ToLittleEndianHex(qint32 value, qint16 digitCount);				//将一个int32转换成小端hex

	//重新做的消息截断，直接处理
	qint8 processNumber(qint8 number);
	QByteArray CRC32_4_imu(const QByteArray& ba);

	QString getBigEndFlt(const QByteArray& data);
	QString getLittleEnd(const QByteArray& data);

	/// <summary>
/// 提供消息码和消息体，对串口发送消息
/// </summary>
/// <param name="func">消息码</param>
/// <param name="body">消息体</param>
/// <returns>发送结果</returns>
	bool writeCMD(const QByteArray& func, const QByteArray& body);
#pragma endregion


#pragma region CMD
	void TestCMD();
	void resetTimeCMD();

	void startGrappingCMD(bool blnStart);		bool tblnGrapping = false;			//开始采集数据,此处相机开始旋转
	void setCameraEnableCMD(QList<qint32> list_enable, QList<qint32> mTriggerMode);	//设置两个任务，1是相机使能模式，2是里程触发（此项无效）
	void setCameraEnableCMD(bool blnEnable);			bool tblnEnable = false;	//设置使能模式只设置开启就可以了
	void setLightCMD(bool blnOpen, qint32 nlight = 0);	bool tblnLight = false;		//设置光源,nlight代表哪个灯是开的，nlight为0
	void getLightAndFansStateCMD();													//获取光源状态'
	void setLightExposureCMD(qint32 nExposure);		qint32 tnExposure = 0;			//设置光源曝光时间
	void setCloverSpeedCMD(qint32 nSpeed);			qint32 tnCloverSpeed = 0;		//设置Clover旋转速度
	void setClovereCamPositionCMD(QList<qint32> list_position); QList<qint32> tlist_position;//设置Clover相机位置
	void startTrolleyCMD(bool blnCleanMileage = true);								//开始小车运行,参数代表是否清除里程,blnTrolleyRunning代表是否正在跑动
	void stopTrolleyCMD(bool blnCleanMileage = true);								//停止小车运行
	void switchTrolleyDirectionCMD(bool blnDirection);	bool tblnDirection = false;	//切换小车运行方向,true为前进，false为后退
	void setTrolleySpeedCMD(qint32 speed);		qint32 tntrolleyspeed = 0;			//获取相机时间
	void getTrolleyStatuCMD();														//获取小车状态
	void setScannerPowerCMD(bool blnPower);		bool tblnScannerPower = false;		//设置扫描仪电源,缓存一个值记录最后一次发送了什么指令给单片机
	void getBatteryUsageCMD();														//获取电池使用情况
	void setEncoderCMD(qint32 nEncoder);	qint32 tnEncoder = 0;										//设置使用编码器，1是左侧编码器 2是右侧编码器 3是双边编码器，目前用双编码器
	void getScannerTimeUpdateCMD();													//获取扫描仪时间
	void resetCloverPositionCMD(qint32 position);									//控制码盘位置到1号位置

#pragma region ZS_CMD
	void ZS_OpenLightCMD(bool blnLight, quint8 nlight = 0);
	void ZS_setCameraEnableCMD(bool blnEnable);
#pragma endregion

#pragma endregion

#pragma region RET
private:
	void TestRET(const QByteArray& arr);
	void resetTimeRET(const QByteArray& ret);
	void startGrappingRET(const QByteArray& ret);									//当前Clover是否正在旋转
	void setCameraEnableRET(const QByteArray& ret);
	void setLightRET(const QByteArray& ret);										//当前光源是否打开
	void getLightAndFansStateRET(const QByteArray& ret);
	void setLightExposureRET(const QByteArray& ret);
	void setCloverSpeedRET(const QByteArray& ret);									//当前Clover旋转速度
	void setClovereCamPositionRET(const QByteArray& ret);							//设置Clover相机位置
	void getCameraTimeUpdateRET(const QByteArray& ret);								//获取相机时间
	void getScannerTimeUpdateRET(const QByteArray& ret);							//获取扫描仪-里程时间，主要用于时间整合
	void startTrolleyRET(const QByteArray& ret);									//开始扫描仪扫描
	void stopTrolleyRET(const QByteArray& ret);
	void switchTrolleyDirectionRET(const QByteArray& ret);							//切换小车运行方向结果
	void setTrolleySpeedRET(const QByteArray& ret);									//设置小车速度
	void getTrolleyStatuRET(const QByteArray& ret);									//获取小车状态
	void setScannerPowerRET(const QByteArray& ret);									//当前扫描仪电源是否打开
	void getBatteryUsageRET(const QByteArray& ret);									//获取电池使用情况
	void setEncoderRET(const QByteArray& ret);										//设置使用编码器，1是左侧编码器 2是右侧编码器 3是双边编码器，目前用双编码器)
	void getDoubleEncoderMileageInfoRET(const QByteArray& ret);						//获取双里程信息
	void getSingleEncoderMileageInfoRET(const QByteArray& ret);	qint64 lastTime = 0;//获得单里程计时间
	void getInclinometerInfoRET(const QByteArray& ret);								//获取倾角仪信息
	void getHardwareButtonInfomationRET(const QByteArray& ret);						//获取物理按键信息，一键停车
	void getIMUMessage(const QByteArray& ret);										//IMU
#pragma endregion



};




