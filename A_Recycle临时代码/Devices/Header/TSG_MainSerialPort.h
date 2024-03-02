#include "PublicVar.h"
#include "TSG_Framework.h"
#include "qserialport.h"
#include "SerialParams.h"
#include "qobject.h"

class SerialPortController : public TSG_Framework {
	Q_OBJECT
public:
	SerialPortController(QObject* parent);
private:
	const QString str_classname = "SerialPortController";
};

//串口初始化参数
//class SerialPortInitParam {
//public:
//	enum class SerialPortType {
//		MS101,			//小车，MS101
//		ZS_Whole,		//全幅设备
//		ZS_Half,		//半幅设备
//		HS_VTM,			//车载设备
//		MS201			//三叶草码盘
//	};
//
//	void SetSerialPortType(SerialPortType type) {
//		SP_Type = type;
//		//暂时不做处理
//	}
//	SerialPortType SP_Type = SerialPortType::MS101;
//	double dbCarDeta = 0.000084368943;				//小车设置比率
//	qint32 nTimeFactory = 10;		//时间倍数，不同的设备的时间是不同的，默认时间基准为1us,也就是所有	输出的时间*时间系数 的单位应该是1us
//	QString portName = "NULL";
//	int baudRate = 921600;
//	QSerialPort::DataBits dataBits = QSerialPort::Data8;
//	QSerialPort::StopBits stopBits = QSerialPort::OneStop;
//	QSerialPort::Parity parity = QSerialPort::NoParity;
//
//	static int getBuadRate(int n)
//	{
//		switch (n)
//		{
//		case 1200: return QSerialPort::Baud1200;
//		case 2400: return QSerialPort::Baud2400;
//		case 4800: return QSerialPort::Baud4800;
//		case 9600: return QSerialPort::Baud9600;
//		case 19200: return QSerialPort::Baud19200;
//		case 38400: return QSerialPort::Baud38400;
//		case 57600: return QSerialPort::Baud57600;
//		case 115200: return QSerialPort::Baud115200;
//		case 460800: return 460800;
//		default: return 921600;
//		}
//	}
//
//	static QSerialPort::DataBits getDataBits(int n)
//	{
//		switch (n)
//		{
//		case 5: return QSerialPort::Data5;
//		case 6: return QSerialPort::Data6;
//		case 7: return QSerialPort::Data7;
//		case 8: return QSerialPort::Data8;
//		}
//		return QSerialPort::Data8;
//	}
//
//	static QSerialPort::StopBits getStopBits(int n)
//	{
//		switch (n)
//		{
//		case 1: return QSerialPort::OneStop;
//		case 2: return QSerialPort::TwoStop;
//		case 3: return QSerialPort::OneAndHalfStop;
//		}
//		return QSerialPort::OneStop;
//	}
//
//	static QSerialPort::Parity getParity(int n)
//	{
//		switch (n)
//		{
//		case 0: return QSerialPort::NoParity;
//		case 2: return QSerialPort::EvenParity;
//		case 3: return QSerialPort::OddParity;
//		case 4: return QSerialPort::SpaceParity;
//		case 5: return QSerialPort::MarkParity;
//		}
//		return QSerialPort::NoParity;
//	}
//
//	static int getBuadRate(QSerialPort::BaudRate n)
//	{
//		switch (n)
//		{
//		case QSerialPort::Baud1200: return 1200;
//		case QSerialPort::Baud2400: return 2400;
//		case QSerialPort::Baud4800: return 4800;
//		case QSerialPort::Baud9600: return 9600;
//		case QSerialPort::Baud19200: return 19200;
//		case QSerialPort::Baud38400: return 38400;
//		case QSerialPort::Baud57600: return 57600;
//		case QSerialPort::Baud115200: return 115200;
//		default: return 921600;
//		}
//		return 19200;
//	}
//
//	static int getDataBits(QSerialPort::DataBits n)
//	{
//		switch (n)
//		{
//		case QSerialPort::Data5: return 5;
//		case QSerialPort::Data6: return 6;
//		case QSerialPort::Data7: return 7;
//		case QSerialPort::Data8: return 8;
//		default: return -1;
//		}
//		return 8;
//	}
//
//	static int getStopBits(QSerialPort::StopBits n)
//	{
//		switch (n)
//		{
//		case QSerialPort::OneStop: return 1;
//		case QSerialPort::TwoStop: return 2;
//		case QSerialPort::OneAndHalfStop: return 3;
//		default: return -1;
//		}
//		return 1;
//	}
//
//	static int getParity(QSerialPort::Parity n)
//	{
//		switch (n)
//		{
//		case QSerialPort::NoParity: return 0;
//		case QSerialPort::EvenParity: return 2;
//		case QSerialPort::OddParity: return 3;
//		case QSerialPort::SpaceParity: return 4;
//		case QSerialPort::MarkParity: return 5;
//		default: return -1;
//		}
//		return 0;
//	}
//
//};
/// <summary>
/// 串口模块，管理串口的收发读写等功能
/// </summary>



class SerialPortController : public TSG_Framework {
	Q_OBJECT
public:
	SerialPortController(QObject* parent, SerialPortInitParam init);
	~SerialPortController();

	bool InitSerialPort(SerialPortInitParam init);

	bool WriteMessage(const QByteArray& message);
private:
	SerialPortInitParam InitParams;
	const QString str_classname = "SerialPortController";
	/// <summary>
	/// 控制串口的指针
	/// </summary>
	QSerialPort* m_serialPort;
	void CheckPortContains();
	bool blnInitSuccess = false;
	/// <summary>
	/// 串口消息的缓冲区，因为可能存在截断的情况，所以需要缓冲区
	/// </summary>
	QByteArray buffer;


	QMap<QByteArray, qint32> map_LengthSp;
	/// <summary>
	/// 初始化指令长度对象
	/// </summary>
	void Init_map_LengthSp();
	void InitDefaultParamsByDeviceName();
	qint8 GetFunctionLength(QByteArray type);	//根据消息码找到长度 
	/// <summary>
	/// 解析buffer内的数据
	/// </summary>
	/// <returns></returns>
	QList<SerialMessage> parseByteArray();

Q_SIGNALS:
	void Sig_SerialPortInitSuccess();
	void Sig_SerialPortMessage(const QByteArray& arr);
};

/ <summary>
/ 串口模块，
/ </summary>
class TSG_MainSerialPort : public TSG_Device<SerialPortInitParam, MissionContent>, public TSG_Framework {
public:
	TSG_MainSerialPort(QObject* parent);
	bool Init(SerialPortInitParam param) override;
	bool SettingMission(MissionContent param) override;
	bool PrewarmMachine() override;
	bool StartMission() override;
	bool PauseMission() override;
	bool EndMission() override;
	bool AnalyseResult() override;
	~TSG_MainSerialPort() {  };
private:
	QString str_classname = SerialPort;
};

class Tester : public TSG_Device<SerialPortInitParam, MissionContent>, public TSG_Framework {

public:
	Tester(QObject* parent, const QString str_classname);
	~Tester() {  };
	bool Init(SerialPortInitParam param) override;
	bool SettingMission(MissionContent param) override;
	bool PrewarmMachine() override;
	bool StartMission() override;
	bool PauseMission() override;
	bool EndMission() override;
	bool AnalyseResult() override;
};