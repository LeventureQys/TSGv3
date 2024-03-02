#include "DeviceSDK.h"

SerialPort::SerialPort(QObject* obj) : TSG_Framework(obj, str_classname)
{
}
SerialPort::~SerialPort()
{
}

bool SerialPort::InitSerialPort(SerialPortInitParam init)
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
		connect(this->m_serialPort, &QSerialPort::readyRead, this, &SerialPort::CheckPortContains);
	}
	return true;
}

bool SerialPort::WriteMessage(const QByteArray& message)
{
	if (this->m_serialPort == nullptr) {
		qDebug() << "WriteMessage - serialPort not init yet";
		return false;
	}

	if (!this->m_serialPort->isOpen()) {
		qDebug() << "WriteMessage - serialPort not open yet";
		return false;
	}
	return true;
}
void SerialPort::CheckPortContains()
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

void SerialPort::Init_map_LengthSp()
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

void SerialPort::InitDefaultParamsByDeviceName()
{
	//此函数主要是用来修改map_LengthSp，也就是接受串口信号的长度，因为不同设备同名串口长度不一
	//根据初始化参数中的设备类型来进行一定的修改
}
qint8 SerialPort::GetFunctionLength(QByteArray type)
{
	if (!this->map_LengthSp.contains(type)) {
		return -1;
	}

	return this->map_LengthSp.value(type);
}

QList<QByteArray> SerialPort::parseByteArray()
{
	QByteArray tBuffer = this->buffer;
	this->buffer.clear();

	QByteArray startPattern = c_head;
	QByteArray endPattern = c_tail;
	//在arr中找到所有的帧头
	// 设置要查找的字节序列
	qint32 index = 0;
	QList<qint32> positions;

	QList<QByteArray> serialMessagesRET;

	//找到所有的头
	while ((index = tBuffer.indexOf(startPattern, index)) != -1) {
		positions.append(index);
		index += startPattern.length();
	}

	qint32 index_ = 0;
	qint32 pointer = 0;//截断处指向的具体位置
	for (auto it = positions.begin(); it != positions.end(); ++it) {
		index_ = *it;
		//获取功能码
		QByteArray mCode = tBuffer.mid(index_ + 3, 1);
		qint8 length = this->GetFunctionLength(mCode);

		//如果长度为-1，说明这个消息码是无效的
		if (length == -1) {
			this->CallErrorMessage("#parseByteArray - invalid message code");
			continue;
		}
		QByteArray message = tBuffer.mid(index_, length);
		if (!message.endsWith(endPattern)) {
			this->CallErrorMessage("#parseByteArray - message not end with tail");
			pointer = index_;
			continue;
		}
		else {
			pointer = index_ + length;
		}
		serialMessagesRET.append(message);
	}
	if (pointer < tBuffer.size()) {
		QByteArray rest = tBuffer.mid(pointer, tBuffer.size() - pointer);
		this->buffer.append(rest);
	}

	return serialMessagesRET;
}