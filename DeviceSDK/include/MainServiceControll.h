#include "DeviceSDK.h"
#include "qdir.h"
class MainServiceParam {
public:
	MainServiceType type = MainServiceType::MS101;
	QString ApplicationPath;
};
struct SerialPortThread {
	QThread* thread = nullptr;
	serialPort* port = nullptr;
	SerialPortInitParam* param = nullptr;
};
class MainServiceControll :public TSG_Device<MainServiceParam, MissionContent> {

	Q_OBJECT

public:

	MainServiceControll(QObject* obj);
	~MainServiceControll();

	bool Init(MainServiceParam param);
	bool SettingMission(const MissionContent& param);

	bool PrewarmMachine();

	bool StartMission();

	bool PauseMission();

	bool EndMission();

	bool AnalyseResult();

	QString getDeviceType();

	DeviceState getDevice();

	MainServiceParam getInitParam();

	MissionContent getMissionParams();

	/// <summary>
	/// 尝试捕获串口
	/// </summary>
	/// <returns></returns>
	bool SearchSerialPorts();

private:

	QString str_classname = "MainServiceControll";
	MainServiceParam param;
	MissionContent mission;


	QMutex mutex;
	/// <summary>
	/// key:串口名称 value:串口控制指针
	/// </summary>
	QMap<QString, serialPort*> map_normal;
	/// <summary>
	/// key:串口名称 value:串口控制指针
	/// </summary>
	QMap<QString, serialPort*> map_imu;
	/// <summary>
	/// 候选串口，等待串口返回指令
	/// </summary>
	QMap<QString, serialPort*> map_listeners;

	QString strTaskPath;
	QString strPicsPath;


	void SerialPortFetchSuccess(const SerialPortInitParam& initParams, const serialPort::SerialPortType& type);
	QString getSerialPortName(const serialPortState& state);
	/// <summary>
	/// 获得Update消息内容，并进行分发或者处理
	/// </summary>
	/// <param name="message"></param>
	/// <param name="code"></param>
	/// <param name="obj"></param>
	void UpdateMessage(const serialPortState& message, const QByteArray& code, const QList<QVariant>& obj);
	void SerialPortMessage(const QByteArray& code, const QVariant& message);


	//初始化任务路径
	bool InitMission();
	QFile* file_mileage = nullptr;						//里程文件
	QFile* file_scannerTime = nullptr;					//扫描仪时间信息文件
	QFile* file_singleMileage = nullptr;				//单里程文件
	QFile* file_centralPosition = nullptr;				//照片拍照信息文件
	QFile* file_taskMessage = nullptr;					//任务信息文件
	QFile* file_Inclinometer = nullptr;					//倾角计文件
	QFile* file_FaroFileInfo = nullptr;					//法如文件信息
	QFile* file_Imu = nullptr;							//IMU文件




	//消息处理函数
	void CameraTimeUpdateHandler(const QList<QVariant>& obj);
	void ScannerTimeUpdateHandler(const QList<QVariant>& obj);
	void TrolleyStatusHandler(const QList<QVariant>& obj);
	void BatteryStateHandler(const QList<QVariant>& obj);
	void SingleEncoderMileageUpdateHandler(const QList<QVariant>& obj);
	void DoubleEncoderMileageUpdateHandler(const QList<QVariant>& obj);
	void InclinometerMessageUpdateHandler(const QList<QVariant>& obj);
	void IMUUpdateHandler(const QList<QVariant>& obj);

	/// <summary>
	/// 初始化并打开一个文件
	/// </summary>
	/// <param name="filepath"></param>
	/// <param name="file"></param>
	/// <returns></returns>
	bool InitFile(const QString& filepath, QFile* file);


	bool isPictureTaker();	//判断当前设备是否需要拍照

	void CallMainProcess(const QString& sDescribe, const QString& message);
	void CallDeviceManager(const QString& sDescribe, const QString& message);
};