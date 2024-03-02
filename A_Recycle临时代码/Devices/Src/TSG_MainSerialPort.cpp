#include "TSG_MainSerialPort.h"
SerialPortController::SerialPortController(QObject* parent, SerialPortInitParam init) : TSG_Framework(parent, this->str_classname)
{
	this->InitSerialPort(init);
}

SerialPortController::~SerialPortController()
{
	if (this->m_serialPort != nullptr) {
		this->m_serialPort->close();
		delete this->m_serialPort;
		this->m_serialPort = nullptr;
	}
}

bool SerialPortController::InitSerialPort(SerialPortInitParam init)
{
	this->InitParams = init;
	this->m_serialPort = new QSerialPort(this);
	this->m_serialPort->setPortName(init.portName);
	this->m_serialPort->setBaudRate(init.baudRate);
	this->m_serialPort->setDataBits((QSerialPort::DataBits)init.dataBits);
	this->m_serialPort->setParity((QSerialPort::Parity)init.parity);
	this->m_serialPort->setStopBits((QSerialPort::StopBits)init.stopBits);

	if (this->m_serialPort->open(QIODevice::ReadWrite)) {
		//这里不能做同步等待，因为串口的初始化是一个异步的过程
		//绑定信号
		connect(this->m_serialPort, &QSerialPort::readyRead, this, &SerialPortController::CheckPortContains);
	}
	return true;
}
void SerialPortController::Init_map_LengthSp()
{
	//为什么可以这么做？因为同一台设备上的指令码是不会有冲突的，但是对于不同的设备类型而言，消息码的模式又是不一样的
//可能会出现各种各样的情况，所以需要做区分
//默认是三叶草设备组的消息码模式

	this->map_LengthSp = { {type_getCameraTimeUpdate, 23} ,{type_test, 9} ,
		{type_resetTime, 23} ,{type_CameraEnable, 9} ,
	{type_Light, 9} ,{type_getLightAndFansState, 12} ,
		{type_setLightExposureTime, 9} ,{type_SetCloverSpeed, 9} ,
		{type_setCloverGrappingPosition, 9} ,{type_getScannerTimeUpdate, 24} ,
		{type_StartTrolley, 9} ,{type_StopTrolley, 9} ,
		{type_SwitchDirection, 9} ,{type_SetSpeed, 9} ,
		{type_getTrolleyStatus, 32} ,{type_setScannerPower, 9} ,
		{type_getBatteryState, 30} ,{type_switchEncoder, 9} ,
		{type_doubleEncoderMileageUpdate, 42} ,{type_InclinometerMessage, 24} ,{type_singleEncoderMileageUpdate, 25},
		{type_HardwareButtonInfomation,10}
	};
	//根据设备类型对一些基本数据进行初始化
	this->InitDefaultParamsByDeviceName();
}
void SerialPortController::InitDefaultParamsByDeviceName()
{
	//此函数主要是用来修改map_LengthSp，也就是接受串口信号的长度，因为不同设备同名串口长度不一
	//根据初始化参数中的设备类型来进行一定的修改

}

void SerialPortController::CheckPortContains()
{
	this->buffer.append(this->m_serialPort->readAll());




	QByteArray data = this->m_serialPort->readAll();

	//if (!blnInitSuccess) {
	//	emit this->Sig_SerialPortInitSuccess();
	//	this->blnInitSuccess = true;
	//}

	////向外发送串口消息内容
	//emit this->Sig_SerialPortMessage(data);

}
qint8 SerialPortController::GetFunctionLength(QByteArray type)
{
	if (!this->map_LengthSp.contains(type)) {
		return -1;
	}

	return this->map_LengthSp.value(type);
}
/// <summary>
/// 尝试对buffer的内容做解析
/// </summary>
/// <returns></returns>
QList<SerialMessage> SerialPortController::parseByteArray()
{
	QByteArray arr = this->buffer;
	this->buffer.clear();

	QByteArray startPattern = c_head;
	QByteArray endPattern = c_tail;
	//在arr中找到所有的帧头
	// 设置要查找的字节序列
	qint32 index = 0;
	QList<qint32> positions;

	//找到所有的头
	while ((index = arr.indexOf(startPattern, index)) != -1) {
		positions.append(index);
		index += startPattern.length();
	}

	for (auto it = positions.rbegin(); it != positions.rend(); ++it) {
		qint32 index_ = *it;
		//获取功能码
		QByteArray mCode = buffer.mid(index_ + 3, 1);
		qint8 length = this->GetFunctionLength(mCode);

		if (length == -1) {

		}
	}
	return QList< SerialMessage>();

}

bool SerialPortController::WriteMessage(const QByteArray& message)
{
	if (this->m_serialPort == nullptr) {
		qDebug() << "WriteMessage - serialPort not init yet";
		return false;
	}

	if (!this->m_serialPort->isOpen()) {
		qDebug() << "WriteMessage - serialPort not open yet";
		return false;
	}



}

TSG_MainSerialPort::TSG_MainSerialPort(QObject* parent) : TSG_Device(str_classname), TSG_Framework(parent, str_classname)
{
}

bool TSG_MainSerialPort::Init(SerialPortInitParam param)
{
	return false;
}
bool TSG_MainSerialPort::SettingMission(MissionContent param)
{
	return false;
}
bool TSG_MainSerialPort::PrewarmMachine()
{
	return false;
}
bool TSG_MainSerialPort::StartMission()
{
	return false;
}
bool TSG_MainSerialPort::PauseMission()
{
	return false;
}
bool TSG_MainSerialPort::EndMission()
{
	return false;
}
bool TSG_MainSerialPort::AnalyseResult()
{
	return false;
}



