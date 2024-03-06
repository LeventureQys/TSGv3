#include "DeviceSDK.h"
template <typename T>
T qintToLittleEndian(T value)
{
	T result;
	uint8_t* data = reinterpret_cast<uint8_t*>(&value);
	uint8_t* resultData = reinterpret_cast<uint8_t*>(&result);
	for (int i = 0; i < sizeof(T); ++i) {
		resultData[i] = data[sizeof(T) - i - 1];
	}
	return result;
}
serialPort::serialPort(QObject* obj) : TSG_Framework(obj, str_classname)
{}
serialPort::~serialPort()
{}

bool serialPort::InitSerialPort(const SerialPortInitParam& init)
{
	this->InitParams = init;
	this->m_serialPort = new QSerialPort(this);
	this->m_serialPort->setPortName(init.serialInfo.portName());
	this->m_serialPort->setBaudRate(921600);
	this->m_serialPort->setDataBits(QSerialPort::Data8);
	this->m_serialPort->setParity(QSerialPort::NoParity);
	this->m_serialPort->setStopBits(QSerialPort::OneStop);
	if (this->m_serialPort->open(QIODevice::ReadWrite)) {
		//这里不能做同步等待，因为串口的初始化是一个异步的过程
		//绑定信号
		this->TestCMD();
		connect(this->m_serialPort, &QSerialPort::readyRead, this, &serialPort::CheckPortContains);
	}
	this->Init_map_LengthSp();
	return true;
}

void serialPort::CheckPortContains()
{
	this->buffer.append(this->m_serialPort->readAll());
	QByteArray data = this->m_serialPort->readAll();
	this->queue_byte = this->parseByteArray();
	this->ProcessBuffer();
}

void serialPort::ProcessBuffer()
{
	//处理queue_byte内的数据
	//拷贝queue_byte内的数据，并清理queue_byte
	QQueue<QByteArray> ret = this->queue_byte;
	this->queue_byte.clear();
	if (ret.size() == 0) {
		return;
	}
	for (QByteArray item : ret) {
		if (item.size() == 0) continue;
		SerialMessage message = splitMessage(item);
		if (!blnInitSuccess) {
			//对外发射信号
			if (!message.blnValid) {
				//执行指定函数
				if (this->map_ret.contains(message.type))
					this->map_ret.value(message.type)(message.contain);
			}
		}
		else {
			this->CallErrorMessage("ProcessBuffer - InvalidMessage");
		}
	}
}

SerialMessage serialPort::splitMessage(const QByteArray& message)
{
	QByteArray example = message;
	qDebug() << "##example : " << example.toHex(' ') << "PortName" << this->InitParams.serialInfo.portName();
	if (example.size() < 4) {
		// 处理长度小于4的情况，可能是错误的消息
		return SerialMessage();
	}
	QByteArray checker = example.chopped(4);    //将最后两个字节和crc校验去掉，用作crc校验判断的依据
	//一般串口消息的长度不会有很大的问题，因为每次串口消息都是一组一组的，只需要判断头尾是否正确即可，如果不正确，则抛弃此条消息
	if (!example.startsWith(c_head) || !example.endsWith(c_tail)) {
		//如果不符合要求
		return SerialMessage();
	}
	//如果合法
	example.remove(0, c_head.length());
	example.chop(c_tail.length());
	SerialMessage ret;
	ret.type = example.left(1);
	ret.crc = example.right(2);
	ret.contain = example.mid(1, example.length() - 3);
	//获得了example之后还需要对整体进行一个校验
	if (ret.crc == this->CRC16_2(checker)) {
		ret.blnValid = true;
	}
	return ret;
}

void serialPort::Init_map_LengthSp()
{
	//为什么可以这么做？因为同一台设备上的指令码是不会有冲突的，但是对于不同的设备类型而言，消息码的模式又是不一样的
//可能会出现各种各样的情况，所以需要做区分
//默认是三叶草设备组的消息码模式

	this->map_LengthSp = { {type_getCameraTimeUpdate, 23} ,{type_test, 8} ,
		{type_resetTime, 23} ,{type_CameraEnable, 9} ,
	{type_Light, 9} ,{type_getLightAndFansState, 12} ,
		{type_setLightExposureTime, 9} ,{type_SetCloverSpeed, 9} ,
		{type_setCloverGrappingPosition, 9} ,{type_getScannerTimeUpdate, 24} ,
		{type_StartTrolley, 9} ,{type_StopTrolley, 9} ,
		{type_SwitchDirection, 9} ,{type_SetSpeed, 9} ,
		{type_getTrolleyStatus, 32} ,{type_setScannerPower, 9} ,
		{type_getBatteryState, 30} ,{type_switchEncoder, 9} ,
		{type_doubleEncoderMileageUpdate, 42} ,{type_InclinometerMessage, 24} ,{type_singleEncoderMileageUpdate, 25},
		{type_HardwareButtonInfomation,10},{type_ZS_IMU,60}
	};
	//根据设备类型对一些基本数据进行初始化
	this->InitDefaultParamsByDeviceName();
}

void serialPort::InitDefaultParamsByDeviceName()
{
	//此函数主要是用来修改map_LengthSp，也就是接受串口信号的长度，因为不同设备同名串口长度不一
	//根据初始化参数中的设备类型来进行一定的修改
	if (this->InitParams.SP_Type == MainServiceType::ZS_CRTG) {
		this->map_LengthSp.insert(type_ZS_Mileage, 25);
		this->map_LengthSp.insert(type_ZS_IMU, 61);
		this->map_LengthSp.insert(type_ZS_getStatus, 23);
	}
	else if (this->InitParams.SP_Type == MainServiceType::ZS_Whole) {
		this->map_LengthSp.insert(type_ZS_getCameraTimeUpdate, 22);
	}
}

QByteArray serialPort::CRC16_2(const QByteArray& ba)
{
	uchar* buf = (uchar*)ba.data();
	int len = ba.size();
	unsigned int crc = 0xFFFF;
	for (int pos = 0; pos < len; pos++)
	{
		crc ^= (unsigned int)buf[pos]; // XOR byte into least sig. byte of crc
		for (int i = 8; i != 0; i--) // Loop over each bit
		{
			if ((crc & 0x0001) != 0) // If the LSB is set
			{
				crc >>= 1; // Shift right and XOR 0xA001
				crc ^= 0xA001;
			}
			else // Else LSB is not set
			{
				crc >>= 1; // Just shift right
			}
		}
	}
	// 高低字节转换
	crc = ((crc & 0x00ff) << 8) | ((crc & 0xff00) >> 8);
	// 构造QByteArray来保存CRC结果
	QByteArray crcResult;
	crcResult.append((uchar)(crc >> 8));
	crcResult.append((uchar)crc);
	return crcResult;
}

qint8 serialPort::GetFunctionLength(const QByteArray& type)
{
	if (!this->map_LengthSp.contains(type)) {
		return -1;
	}

	return this->map_LengthSp.value(type);
}

QQueue<QByteArray> serialPort::parseByteArray() {
	QByteArray tBuffer = this->buffer;
	this->buffer.clear();

	QByteArray startPattern = c_head;
	QByteArray endPattern = c_tail;
	//在arr中找到所有的帧头
	// 设置要查找的字节序列
	qint32 index = 0;
	QList<qint32> positions;

	QQueue<QByteArray> serialMessagesRET;

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

bool serialPort::writeData(const QByteArray& data)
{
	if (this->m_serialPort == nullptr || !this->m_serialPort->isOpen()) {
		this->CallErrorMessage("writeData - serialPort not open yet");
		return false;
	}

	QByteArray arr = data;
	qint32 nRet = -1;
	//不知道为什么，反正这里就是要等一秒，不然有些串口数据写进去就是会丢失
	QThread::sleep(1);
	nRet = this->m_serialPort->write(arr);
	if (!this->m_serialPort->waitForBytesWritten(10000)) {
		this->CallErrorMessage("writeData - waitForBytesWritten timeout");
		return false;
	}

	this->m_serialPort->flush();
	if (nRet == -1) {
		this->CallErrorMessage("writeData - write failed");
		return false;
	}
	QThread::sleep(1);
	qDebug() << "## SendTest : " << arr.toHex(' ').toUpper();
	return true;
}

bool serialPort::writeCMD(const QByteArray& func, const QByteArray& body)
{
	QByteArray allArr;
	allArr.append(c_head);
	allArr.append(func);
	allArr.append(body);
	allArr.append(CRC16_2(allArr));
	allArr.append(c_tail);
	return this->writeData(allArr);
}


bool serialPort::SetMission(const MissionContent& mission)
{

	return false;
}

void serialPort::registFunctions()
{
}
QByteArray serialPort::PositionToHexadecimal(const QList<qint32>& enabledCameras) {
	// 创建一个初始值为0的两字节整数，表示相机使能位
	quint16 cameraEnable = 0;

	// 根据enabledCameras列表设置相应的位
	if (enabledCameras.size() == 0) {
		return QByteArray::fromHex("0000");
	}
	for (int camera : enabledCameras) {
		if (camera >= 1 && camera <= 16) {
			// 设置相应的相机使能位
			cameraEnable |= (1 << (camera - 1));
		}
		else {
			qDebug() << "Invalid camera number: " << camera;
		}
	}

	// 构建QByteArray指令
	QByteArray command;
	command.append(static_cast<char>(cameraEnable & 0xFF));  // 低位
	command.append(static_cast<char>(cameraEnable >> 8));     // 高位

	return command;
}
QByteArray serialPort::int32ToLittleEndianHex(qint32 value, qint16 digitCount)
{
	QByteArray byteArray;
	byteArray.resize(digitCount);

	for (int i = 0; i < digitCount; i += 2) {
		byteArray[i] = static_cast<char>(value & 0xFF);
		byteArray[i + 1] = static_cast<char>((value >> 8) & 0xFF);
		value >>= 16;
	}

	return byteArray;
}
qint8 serialPort::processNumber(qint8 number) {
	const qint8 minVal = 1;
	const qint8 maxVal = 14;

	// Use modulus operator to wrap the number within the range
	number = ((number - 1) % maxVal) + 1;

	return number;
}

void serialPort::TestCMD()
{
	this->writeCMD(QByteArray::fromHex("00"), QByteArray());
}
void serialPort::resetTimeCMD()
{
	this->writeCMD(type_resetTime, QByteArray());
}

void serialPort::startGrappingCMD(bool blnStart)
{
	//是否启动采集
	QByteArray contain;
	if (blnStart) {
		contain.append(QByteArray::fromHex("01"));
	}
	else {
		contain.append(QByteArray::fromHex("00"));
	}
	tblnGrapping = blnStart;
	this->writeCMD(type_StartGrapping, contain);
}

void serialPort::setCameraEnableCMD(QList<qint32> list_enable, QList<qint32> mTriggerMode)
{
	//只管使能模式就可以了
	QByteArray qba_enable, qba_trigger;
	qba_enable = this->PositionToHexadecimal(list_enable);
	qba_trigger = this->PositionToHexadecimal(mTriggerMode);

	QByteArray qba_all = qba_enable + qba_trigger;
	this->writeCMD(type_CameraEnable, qba_all);
}

void serialPort::setCameraEnableCMD(bool blnEnable)
{
	//需要两个设置，分别是相机使能和工作模式（里程触发）使能
	QByteArray qba_enable, qba_trigger;
	if (blnEnable) {
		qba_enable = QByteArray::fromHex("FF 03");
		qba_trigger = QByteArray::fromHex("FF 03");
	}
	else {
		qba_enable = QByteArray::fromHex("00 00");
		qba_trigger = QByteArray::fromHex("00 00");
	}
	QByteArray qba_all = qba_enable + qba_trigger;
	this->tblnEnable = blnEnable;
	this->writeCMD(type_CameraEnable, qba_all);
}

void serialPort::setLightCMD(bool blnOpen, qint32 nlight) {
	QByteArray qba_light;
	QByteArray qba_open;

	if (nlight == 0) {
		qba_light = QByteArray::fromHex("00");
	}
	else {
		qba_light = QByteArray::number(nlight, 16);
	}

	if (blnOpen) {
		qba_open = QByteArray::fromHex("01");
	}
	else {
		qba_open = QByteArray::fromHex("00");
	}

	QByteArray arr_all = qba_light + qba_open;
	this->tblnLight = blnOpen;
	this->writeCMD(type_Light, arr_all);
}

void serialPort::getLightAndFansStateCMD() {
	this->writeCMD(type_getLightAndFansState, QByteArray());
}

void serialPort::setLightExposureCMD(qint32 nExposure)
{
	QByteArray contains = this->int32ToLittleEndianHex(nExposure, 2);
	this->tnExposure = nExposure;
	this->writeCMD(type_setLightExposureTime, contains);
}

void serialPort::setCloverSpeedCMD(qint32 nSpeed) {
	QByteArray contains = this->int32ToLittleEndianHex(nSpeed, 2);
	this->tnCloverSpeed = nSpeed;
	this->writeCMD(type_SetCloverSpeed, contains);
}

void serialPort::setClovereCamPositionCMD(QList<qint32> list_position) {
	//这里实际上要设置三个相机的拍照位，但是实际上三个相机的拍照位是一致的，所以只需要一组list
	QByteArray contains = this->PositionToHexadecimal(list_position);
	QByteArray arr_all = contains + contains + contains;
	this->tlist_position = list_position;
	this->writeCMD(type_setCloverGrappingPosition, arr_all);
}

void serialPort::startTrolleyCMD(bool blnCleanMileage) {
	QByteArray contains;
	if (blnCleanMileage) {
		contains.append(QByteArray::fromHex("01"));
	}
	else {
		contains.append(QByteArray::fromHex("00"));
	}
	this->writeCMD(type_StartTrolley, contains);
}

void serialPort::stopTrolleyCMD(bool blnCleanMileage) {
	QByteArray contains;
	if (blnCleanMileage) {
		contains.append(QByteArray::fromHex("01"));
	}
	else {
		contains.append(QByteArray::fromHex("00"));
	}
	this->writeCMD(type_StopTrolley, contains);
}

void serialPort::switchTrolleyDirectionCMD(bool blnDirection) {
	QByteArray contains;
	if (blnDirection) {
		contains.append(QByteArray::fromHex("01"));
	}
	else {
		contains.append(QByteArray::fromHex("00"));
	}
	this->tblnDirection = blnDirection;
	this->writeCMD(type_SwitchDirection, contains);
}

void serialPort::setTrolleySpeedCMD(qint32 nSpeed) {
	QByteArray contains = this->int32ToLittleEndianHex(nSpeed, 2);
	this->tntrolleyspeed = nSpeed;
	this->writeCMD(type_SetSpeed, contains);
}

void serialPort::getTrolleyStatuCMD() {
	this->writeCMD(type_getTrolleyStatus, QByteArray());
}

void serialPort::setScannerPowerCMD(bool blnPower) {
	if (blnPower) {
		this->writeCMD(type_setScannerPower, QByteArray::fromHex("01"));
		tblnScannerPower = blnPower;
	}
	else {
		this->writeCMD(type_setScannerPower, QByteArray::fromHex("00"));
	}
}

void serialPort::getBatteryUsageCMD() {
	this->writeCMD(type_getBatteryState, QByteArray());
}

void serialPort::setEncoderCMD(qint32 nEncoder) {
	QByteArray contains = QByteArray::fromHex(QByteArray::number(nEncoder));
	this->tnEncoder = nEncoder;
	this->writeCMD(type_switchEncoder, contains);
}

void serialPort::getScannerTimeUpdateCMD() {
	this->writeCMD(type_getScannerTimeUpdate, QByteArray());
}

void serialPort::resetCloverPositionCMD(qint32 position) {
	if (position <= 15 && position >= 1) {
		this->writeCMD(type_ResetCloverPosition, QByteArray::fromHex(QString::number(position).toLocal8Bit()));
	}
}

void serialPort::TestRET(const QByteArray& arr)
{
	emit this->Sig_SerialPortInitSuccess(this->InitParams, SerialPortType::Normal);
	this->blnInitSuccess = true;
}

void serialPort::resetTimeRET(const QByteArray& ret)
{
	emit this->Sig_SerialPortMessage(type_resetTime, true);
}

void serialPort::startGrappingRET(const QByteArray& ret)
{
	if (ret.contains(QByteArray::fromHex("00"))) {
		emit this->Sig_SerialPortMessage(type_StartGrapping, true);
		this->state.blnGrapping = this->tblnGrapping;
	}
	else {
		emit this->Sig_SerialPortMessage(type_StartGrapping, false);
	}
}

void serialPort::setCameraEnableRET(const QByteArray& ret)
{
	if (ret.contains(QByteArray::fromHex("00"))) {
		emit this->Sig_SerialPortMessage(type_CameraEnable, true);
		this->state.blnCameraEnable = this->tblnEnable;
	}
	else {
		emit this->Sig_SerialPortMessage(type_CameraEnable, false);
	}

}

void serialPort::setLightRET(const QByteArray& ret)
{
	if (ret.contains(QByteArray::fromHex("00"))) {
		emit this->Sig_SerialPortMessage(type_Light, true);
		this->state.blnLight = this->tblnGrapping;
	}
	else {
		emit this->Sig_SerialPortMessage(type_Light, false);
	}

}

void serialPort::getLightAndFansStateRET(const QByteArray& ret)
{

}

void serialPort::setLightExposureRET(const QByteArray& ret)
{
	if (ret.contains(QByteArray::fromHex("00"))) {
		emit this->Sig_SerialPortMessage(type_setLightExposureTime, true);
		this->state.nExposure = this->tnExposure;
	}
	else {
		emit this->Sig_SerialPortMessage(type_setLightExposureTime, false);
	}
}

void serialPort::setCloverSpeedRET(const QByteArray& ret)
{
	if (ret.contains(QByteArray::fromHex("00"))) {
		emit this->Sig_SerialPortMessage(type_SetCloverSpeed, true);
		this->state.nCloverSpeed = this->tnCloverSpeed;
	}
	else {
		emit this->Sig_SerialPortMessage(type_SetCloverSpeed, false);
	}
}

void serialPort::setClovereCamPositionRET(const QByteArray& ret)
{
	if (ret.contains(QByteArray::fromHex("00"))) {
		emit this->Sig_SerialPortMessage(type_setCloverPosition, true);
		this->state.list_position = this->tlist_position;
	}
	else {
		emit this->Sig_SerialPortMessage(type_setCloverPosition, false);
	}
}

void serialPort::getCameraTimeUpdateRET(const QByteArray& ret)
{
	if (ret.size() < 15) {
		this->CallErrorMessage("#getCameraTimeUpdateRET - QByteArray is too short to parse");
		return;
	}
	// 解析触发次数
	quint32 triggerCount;
	QDataStream triggerStream(ret.left(4));
	triggerStream.setByteOrder(QDataStream::LittleEndian); // 设置为小端字节序
	triggerStream >> triggerCount;

	// 解析时间
	quint64 time;
	QDataStream timeStream(ret.mid(4, 8));
	timeStream.setByteOrder(QDataStream::LittleEndian); // 设置为小端字节序
	timeStream >> time;

	// 解析相机ID
	quint8 cameraID = static_cast<quint8>(ret.at(12));

	// 解析曝光反馈
	quint8 exposureFeedback = static_cast<quint8>(ret.at(13));

	// 解析旋转机构位置
	quint8 rotationPosition = static_cast<quint8>(ret.at(14));

	quint8 actualPosition = -1;

	switch (cameraID) {
	case 1: {
		actualPosition = rotationPosition;
		break;
	}
	case 2: {
		actualPosition = rotationPosition + 10;
		break;
	}
	case 3: {
		actualPosition = rotationPosition + 5;
		break;
	}
		  actualPosition = this->processNumber(actualPosition);
	}

	//QJsonObject obj_ret;
	//obj_ret.insert("ID", QString::number(triggerCount));
	//obj_ret.insert("CamID", QString::number(cameraID));
	//obj_ret.insert("CentralPosition", QString::number(rotationPosition));
	//obj_ret.insert("Time", QString::number(time));
	//obj_ret.insert("ActualPosition", QString::number(actualPosition));
	QString insert = QString("%1|%2|%3|%4")
		.arg(QString::number(triggerCount))
		.arg(QString::number(cameraID))
		.arg(QString::number(rotationPosition))
		.arg(QString::number(time));
	QList<QVariant> list_ret;
	list_ret.append(triggerCount);
	list_ret.append(cameraID);
	list_ret.append(rotationPosition);
	list_ret.append(time);
	this->UpdateMessage(type_getCameraTimeUpdate, list_ret);
}

void serialPort::getScannerTimeUpdateRET(const QByteArray& ret) {
	if (ret.size() < 16) {
		qDebug() << "QByteArray is too short to parse.";
		return;
	}
	this->state.nScannerTimeID++;
	QDataStream stream(ret);
	stream.setByteOrder(QDataStream::LittleEndian);

	quint64 scannerTime;
	quint64 TrolleyTime;

	stream >> scannerTime;
	stream >> TrolleyTime;

	TrolleyTime = TrolleyTime * 10;		//小车的时间单位为1us,扫描仪的时间单位为100ns
	qint64 scannerTime_trans = scannerTime;
	QString insert = QString("%1|%2")
		.arg(QString::number(scannerTime_trans))
		.arg(QString::number(TrolleyTime));
	QList<QVariant> list_ret;
	list_ret.append(scannerTime_trans);
	list_ret.append(TrolleyTime);
	if (scannerTime_trans != 0) {
		this->state.nCurrentTimeDiff = scannerTime_trans - TrolleyTime;
	}
	else {
		this->state.nCurrentTimeDiff = 0;
	}

	this->UpdateMessage(type_getScannerTimeUpdate, list_ret);
}

void serialPort::startTrolleyRET(const QByteArray& ret) {
	if (ret.contains(QByteArray::fromHex("00"))) {
		this->state.blnTrolleyRunning = true;
		emit this->Sig_SerialPortMessage(type_StartTrolley, true);
		return;
	}
	else {
		emit this->Sig_SerialPortMessage(type_StartTrolley, false);
		this->CallErrorMessage("startTrolleyCMD - StartTrolleyFailed");
		return;
	}
}

void serialPort::stopTrolleyRET(const QByteArray& ret) {
	if (ret.contains(QByteArray::fromHex("00"))) {
		this->state.blnTrolleyRunning = false;
		emit this->Sig_SerialPortMessage(type_StopTrolley, true);
		return;
	}
	else {
		emit this->Sig_SerialPortMessage(type_StopTrolley, false);
		this->CallErrorMessage("startTrolleyCMD - StartTrolleyFailed");
		return;
	}
}

void serialPort::switchTrolleyDirectionRET(const QByteArray& ret) {
	if (ret.contains(QByteArray::fromHex("00"))) {
		emit this->Sig_SerialPortMessage(type_SwitchDirection, true);
		this->state.blnDirection = this->tblnDirection;
	}
	else {
		emit this->Sig_SerialPortMessage(type_SwitchDirection, false);
		this->CallErrorMessage("switchTrolleyDirectionRET - sp_switchTrolleyDirectionFailed");
		return;
	}
}

void serialPort::setTrolleySpeedRET(const QByteArray& ret) {
	if (ret.contains(QByteArray::fromHex("00"))) {
		emit this->Sig_SerialPortMessage(type_SetSpeed, true);
		this->state.blnDirection = this->tblnDirection;
	}
	else {
		emit this->Sig_SerialPortMessage(type_SwitchDirection, false);
		this->CallErrorMessage("switchTrolleyDirectionRET - sp_switchTrolleyDirectionFailed");
		return;
	}
}

void serialPort::getTrolleyStatuRET(const QByteArray& ret) {
	if (ret.size() < 16) {
		qDebug() << "QByteArray is too short to parse.";
		return;
	}
	QDataStream stream(ret);
	stream.setByteOrder(QDataStream::LittleEndian);

	stream >> this->state.nTrolleySpeed;
	bool temperatureSymbol;
	quint16 temperatureValue;
	stream >> temperatureSymbol;
	stream >> temperatureValue;
	//因为他是1为负，所以这个要反转一下
	temperatureSymbol = !temperatureSymbol;

	float actualTemperature = 0.0;
	temperatureSymbol ? actualTemperature = temperatureValue / 10.0 : actualTemperature = -1 * temperatureValue / 10.0;
	this->state.nTemperature = actualTemperature;

	stream >> this->state.blnDirection;
	stream >> this->state.blnTrolleyRunning;
	stream >> this->state.nBatteryUsage;
	stream >> this->state.nKnobState;
	stream >> this->state.nEncoder;
	stream >> this->state.nKeyMode;
	stream >> this->state.blnScannerPower;
	stream >> this->state.nMinSpeed;
	stream >> this->state.nMaxSpeed;
	stream >> this->state.lnInternalTime;
	//此处将SerialPort的数据上传
	QList<QVariant> list_ret;
	list_ret.append(this->state.nTemperature);
	list_ret.append(this->state.blnDirection);
	list_ret.append(this->state.nBatteryUsage);
	this->UpdateMessage(type_getTrolleyStatus, list_ret);
}

void serialPort::setScannerPowerRET(const QByteArray& ret) {
	if (ret.contains(QByteArray::fromHex("00"))) {
		//返回成功，则状态修改成功
		emit this->Sig_SerialPortMessage(type_setScannerPower, true);
		this->state.blnScannerPower = this->tblnScannerPower;
		return;
	}
	else {
		emit this->Sig_SerialPortMessage(type_setScannerPower, false);
		this->CallErrorMessage("setScannerPowerRET - setScannerPowerFailed");
		return;
	}
}

void serialPort::getBatteryUsageRET(const QByteArray& ret) {
	QByteArray arr = ret;
	qDebug() << "#arr" << arr.toHex(' ');
	BatteryInfo result_left = BatteryInfo();
	BatteryInfo result_right = BatteryInfo();
	QDataStream stream(arr);
	stream.setByteOrder(QDataStream::BigEndian);  // 设置字节顺序为大端序
	//这里的每个字节组都需要转换成小端序
	stream >> result_left.status;	//第一字节00表示当前状态
	result_left.status = qintToLittleEndian<quint8>(result_left.status);
	stream >> result_left.voltage;
	result_left.voltage = qintToLittleEndian<qint16>(result_left.voltage);
	stream >> result_left.current;
	result_left.current = qintToLittleEndian<qint16>(result_left.current);
	stream >> result_left.remainingCapacity;
	result_left.remainingCapacity = qintToLittleEndian<qint16>(result_left.remainingCapacity);

	stream >> result_left.fullCapacity;
	result_left.fullCapacity = qintToLittleEndian<qint16>(result_left.fullCapacity);
	stream >> result_left.cycleCount;
	result_left.cycleCount = qintToLittleEndian<qint16>(result_left.cycleCount);

	stream >> result_right.status;	//第一字节00表示当前状态
	result_right.status = qintToLittleEndian<quint8>(result_right.status);
	stream >> result_right.voltage;
	result_right.voltage = qintToLittleEndian<qint16>(result_right.voltage);
	stream >> result_right.current;
	result_right.current = qintToLittleEndian<qint16>(result_right.current);
	stream >> result_right.remainingCapacity;
	result_right.remainingCapacity = qintToLittleEndian<qint16>(result_right.remainingCapacity);

	stream >> result_right.fullCapacity;
	result_right.fullCapacity = qintToLittleEndian<qint16>(result_right.fullCapacity);
	stream >> result_right.cycleCount;
	result_right.cycleCount = qintToLittleEndian<qint16>(result_right.cycleCount);
	double percentage_left = static_cast<double>(result_left.remainingCapacity) / static_cast<double>(result_left.fullCapacity);
	double percentage_right = static_cast<double>(result_right.remainingCapacity) / static_cast<double>(result_right.fullCapacity);
	if (std::isnan(percentage_left)) percentage_left = 0.00;
	if (std::isnan(percentage_right)) percentage_right = 0.00;
	double percentage_ret = (percentage_left + percentage_right) / 2;
	QList<QVariant> list_ret;
	list_ret.append(percentage_left);
	list_ret.append(percentage_right);
	list_ret.append(percentage_ret);

	this->UpdateMessage(type_getBatteryState, list_ret);
	return;
}

void serialPort::setEncoderRET(const QByteArray& ret) {
	if (ret.contains(QByteArray::fromHex("00"))) {
		//返回成功，则状态修改成功
		emit this->Sig_SerialPortMessage(type_switchEncoder, true);
		this->state.nEncoder = this->tnEncoder;
		return;
	}
	else {
		emit this->Sig_SerialPortMessage(type_switchEncoder, false);
		this->CallErrorMessage("setEncoderRET - setEncoderFailed");
		return;
	}
}

void serialPort::getDoubleEncoderMileageInfoRET(const QByteArray& ret) {
	if (ret.size() != 34) {
		qDebug() << ret.size();
		qDebug() << "QByteArray size is not 42 bytes.";
		return;
	}
	this->state.nMileageID++;
	QDataStream stream(ret);

	MileageData leftMileage;
	MileageData rightMileage;
	stream.setByteOrder(QDataStream::LittleEndian);

	// 解析左侧里程
	stream >> leftMileage.positive;
	stream >> leftMileage.pulseCount;
	stream >> leftMileage.time;
	leftMileage.time = leftMileage.time * quint64(this->state.initParam.nTimeFactory);
	// 解析右侧里程
	stream >> rightMileage.positive;
	stream >> rightMileage.pulseCount;
	stream >> rightMileage.time;
	rightMileage.time = rightMileage.time * quint64(this->state.initParam.nTimeFactory);
	qint16 sign_left = leftMileage.positive ? -1 : 1;
	qint16 sign_right = rightMileage.positive ? -1 : 1;
	double mileage_left = 0.00;
	double mileage_right = 0.00;
	mileage_left = double(sign_left) * double(leftMileage.pulseCount) * this->state.initParam.dbCarDeta;
	mileage_right = double(sign_right) * double(rightMileage.pulseCount) * this->state.initParam.dbCarDeta;

	qint64 leftTime = (qint64(leftMileage.time) + this->state.nCurrentTimeDiff) < 0 ? 0 : (qint64(leftMileage.time) + this->state.nCurrentTimeDiff);
	qint64 rightTime = (qint64(rightMileage.time) + this->state.nCurrentTimeDiff) < 0 ? 0 : (qint64(rightMileage.time) + this->state.nCurrentTimeDiff);
	QList<QVariant> list_ret;
	list_ret.append(this->state.nMileageID);
	list_ret.append(mileage_left);
	list_ret.append(leftTime);
	list_ret.append((qint64)leftMileage.time);
	list_ret.append(mileage_right);
	list_ret.append(rightTime);
	list_ret.append((qint64)rightMileage.time);

	this->UpdateMessage(type_doubleEncoderMileageUpdate, list_ret);
	this->lastTime = leftMileage.time;
}

void serialPort::getSingleEncoderMileageInfoRET(const QByteArray& data) {
	if (data.size() < 17) {
		return;
	}

	QDataStream stream(data);
	stream.setByteOrder(QDataStream::LittleEndian);
	this->state.nMileageID++;
	quint8 symbol;
	quint64 mileage;
	quint64 time;

	stream >> symbol;
	stream >> mileage;
	stream >> time;
	double mileageValue = 0.00;

	if (symbol == 0x00) {
		mileageValue = double(mileage) * this->state.initParam.dbCarDeta;
	}
	else {
		mileageValue = -1 * double(mileage) * this->state.initParam.dbCarDeta;
	}

	quint64 timeRaw = time * quint64(this->state.initParam.nTimeFactory);

	qint64 timeScanner = timeRaw + quint64(this->state.nCurrentTimeDiff);

	if (timeScanner < 0) timeScanner = 0;
	//返回值的单位为10us，我们需要同步到1us
	QList<QVariant> list_ret;
	list_ret.append(this->state.nMileageID);
	list_ret.append(mileageValue);
	list_ret.append(timeScanner);
	list_ret.append(qint64(timeRaw));
	//两秒传输一次
	if (this->state.nMileageID % 20 == 0) {
		this->CallErrorMessage(QString("MainService - updateMileage"));
	}

	this->lastTime = timeRaw;
	this->UpdateMessage(type_singleEncoderMileageUpdate, list_ret);
}

void serialPort::getInclinometerInfoRET(const QByteArray& ret) {

	//尝试解析倾角计信息
	if (ret.size() < 14) {
		qDebug() << "## getInclinometerInfoRET: QByteArray size is not 14 bytes.";
		return;
	}

	QDataStream stream(ret);
	stream.setByteOrder(QDataStream::LittleEndian);

	InclinometerInfo info;
	stream >> info.X;
	stream >> info.Y;
	stream >> info.time;

	QList<QVariant> list_ret;
	list_ret.append(info.X);
	list_ret.append(info.Y);
	list_ret.append(info.time);


	this->UpdateMessage(type_InclinometerMessage, list_ret);
}

void serialPort::getHardwareButtonInfomationRET(const QByteArray& ret) {
	if (ret.size() < 2) {
		qDebug() << "## getHardwareButtonInfomationRET: QByteArray size is not 2 bytes.";
		return;
	}

	quint8 button;
	quint8 knob;

	QDataStream stream(ret);
	stream >> button;
	stream >> knob;
	if (button == 3) {//停止按钮

	}
	else if (button == 10) {//转向，但是会直接读status那个loop信息

	}
}

void serialPort::UpdateMessage(const QByteArray& code, const QList<QVariant>& obj) {
	emit this->Sig_UpdateMessage(this->state, code, obj);
}

bool serialPort::PrewarmMachine()
{
	return false;
}

QByteArray serialPort::CRC32_4_imu(const QByteArray& ba) {
	unsigned int i;
	unsigned int crc = 0xFFFFFFFF;		//4字节CRC
	int len = ba.size();

	for (int i = 0; i < len; ++i)		//低位到高位遍历待校验数据
	{
		crc ^= (unsigned int)(ba.at(i)) << 24;	//
		for (int j = 0; j < 8; ++j)			//1字节计算
		{
			if (crc & 0x80000000)
				crc = (crc << 1) ^ 0x04C11DB7;
			else
				crc <<= 1;
		}
	}
	char* crcB = new char[4];
	crcB[3] = crc & 0x000000FF;
	crcB[2] = crc >> 8 & 0x000000FF;
	crcB[1] = crc >> 16 & 0x000000FF;
	crcB[0] = crc >> 24 & 0x000000FF;

	QByteArray ret = QByteArray(crcB, 4);
	return ret;
}

QString serialPort::getLittleEnd(const QByteArray& data) {
	if (data.size() > 8) return "";
	qulonglong result = 0;
	for (int i = 0; i < data.size(); ++i)
	{
		qulonglong tmp = (uchar)data[i];
		result += tmp <<= i * 8;
	}
	return QString::number(result);
}

QString serialPort::getBigEndFlt(const QByteArray& data) {
	const int fltLen = 4;
	if (data.size() != fltLen) return "null";
	float result = 0;
	uchar fltArr[fltLen];
	for (int i = 0; i < fltLen; ++i)
	{
		fltArr[fltLen - i - 1] = data.at(i);
	}
	return QString::number(*(float*)fltArr, 'f', 9);
}

void serialPort::getIMUMessage(const QByteArray& data)
{
	if (data.size() < 50) {
		return;
	}

	//取出前36位为惯导数据，8位时间，8位ID，2位结尾
	QByteArray imuData = data.mid(0, 32);
	QByteArray imuCRC = data.mid(32, 4);
	QByteArray time = data.mid(36, 8);
	QByteArray id = data.mid(44, 8);
	ImuRawDataInfo imu_data;

	//检查Imu数据,对Imu做CRC校验，如果CRC不通过则丢弃
	if (!imuCRC.contains(CRC32_4_imu(imuData)))
		return;
	//imu中前四位为头信息
	QByteArray imuHead = imuData.mid(0, 4);
	//5-8为xRotation
	//imu_data.imu.XRotation = getLittleEnd(imuData.mid(4, 4)).toInt();
	imu_data.imu.XRotation = QString::number(getBigEndFlt(imuData.mid(4, 4)).toFloat() / Pai * 180, 'f', 9);
	//9-12 yRotation
	imu_data.imu.YRotation = QString::number(getBigEndFlt(imuData.mid(8, 4)).toFloat() / Pai * 180, 'f', 9);
	//13-16 zRotation
	imu_data.imu.ZRotation = QString::number(getBigEndFlt(imuData.mid(12, 4)).toFloat() / Pai * 180, 'f', 9);
	//17-20 xAcceleration
	imu_data.imu.XAcceleration = getBigEndFlt(imuData.mid(16, 4));
	//21--24 yAcceleration
	imu_data.imu.YAcceleration = getBigEndFlt(imuData.mid(20, 4));
	//25-28 zAcceleration
	imu_data.imu.ZAcceleration = getBigEndFlt(imuData.mid(24, 4));
	imu_data.time = this->getLittleEnd(time).toLongLong() * this->state.initParam.nTimeFactory;
	imu_data.id = this->getLittleEnd(id).toLongLong();

	QList<QVariant> ret;
	ret.append(imu_data.imu.XRotation);
	ret.append(imu_data.imu.YRotation);
	ret.append(imu_data.imu.ZRotation);
	ret.append(imu_data.imu.XAcceleration);
	ret.append(imu_data.imu.YAcceleration);
	ret.append(imu_data.imu.ZAcceleration);
	ret.append(imu_data.time);
	ret.append(imu_data.id);

	this->UpdateMessage(type_ZS_IMU, ret);
}