#include "MainServiceControll.h"

MainServiceControll::MainServiceControll(QObject* obj) : TSG_Device(obj, this->str_classname)
{
}

MainServiceControll::~MainServiceControll()
{
}

bool MainServiceControll::Init(MainServiceParam param)
{
	this->param = param;
	return this->SearchSerialPorts();
}

bool MainServiceControll::SettingMission(const MissionContent& param)
{
	this->mission = param;
	for (auto item : this->map_normal) {
		if (!item->SetMission(param)) return false;
	}

	for (auto item : this->map_imu) {
		if (!item->SetMission(param)) return false;
	}
	return true;
}

bool MainServiceControll::PrewarmMachine()
{
	if (!this->InitMission()) return false;


	for (auto item : this->map_normal) {
		if (!item->PrewarmMachine()) return false;
	}

	for (auto item : this->map_imu) {
		if (!item->PrewarmMachine()) return false;
	}

	return true;
}

bool MainServiceControll::StartMission()
{
	for (auto item : this->map_normal) {
		if (!item->StartMission()) return false;
	}

	for (auto item : this->map_imu) {
		if (!item->StartMission()) return false;
	}

	return true;
}

bool MainServiceControll::PauseMission()
{
	for (auto item : this->map_normal) {
		if (!item->PauseMission()) return false;
	}

	for (auto item : this->map_imu) {
		if (!item->PauseMission()) return false;
	}
	return true;
}

bool MainServiceControll::EndMission()
{
	for (auto item : this->map_normal) {
		if (!item->EndMission()) return false;
	}

	for (auto item : this->map_imu) {
		if (!item->EndMission()) return false;
	}
	return true;
}

bool MainServiceControll::AnalyseResult()
{
	for (auto item : this->map_normal) {
		if (!item->AnalyseResult()) return false;
	}

	for (auto item : this->map_imu) {
		if (!item->AnalyseResult()) return false;
	}
	return true;
}

QString MainServiceControll::getDeviceType()
{
	return QString();
}

DeviceState MainServiceControll::getDevice()
{
	return DeviceState();
}

MainServiceParam MainServiceControll::getInitParam()
{
	return MainServiceParam();
}

MissionContent MainServiceControll::getMissionParams()
{
	return MissionContent();
}

bool MainServiceControll::SearchSerialPorts()
{
	mutex.lock();

	QList<QSerialPortInfo> list_sp = QSerialPortInfo::availablePorts();
	for (qint32 i = 0; i < list_sp.size(); ++i) {
		SerialPortInitParam param;
		param.SP_Type = this->param.type;
		serialPort* port = new serialPort(this);
		connect(port, &serialPort::Sig_SerialPortInitSuccess, this, &MainServiceControll::SerialPortFetchSuccess);
		connect(port, &serialPort::Sig_UpdateMessage, this, &MainServiceControll::UpdateMessage);
		connect(port, &serialPort::Sig_SerialPortMessage, this, &MainServiceControll::SerialPortMessage);
		port->InitSerialPort(param);
		this->map_listeners.insert(param.serialInfo.portName(), port);
	}

	mutex.unlock();
	return true;
}

void MainServiceControll::SerialPortFetchSuccess(const SerialPortInitParam& initParams, const serialPort::SerialPortType& type)
{
	//根据结果分配串口
	if (type == serialPort::SerialPortType::Normal) {
		if (!map_listeners.contains(initParams.serialInfo.portName()))
			return;
		serialPort* port = map_listeners.value(initParams.serialInfo.portName());
		this->map_normal.insert(initParams.serialInfo.portName(), port);
	}
	else if (type == serialPort::SerialPortType::Imu) {
		if (!map_listeners.contains(initParams.serialInfo.portName()))
			return;
		serialPort* port = map_listeners.value(initParams.serialInfo.portName());
		this->map_imu.insert(initParams.serialInfo.portName(), port);
	}
}

QString MainServiceControll::getSerialPortName(const serialPortState& state)
{
	return state.initParam.serialInfo.portName();
}

void MainServiceControll::UpdateMessage(const serialPortState& message, const QByteArray& code, const QList<QVariant>& obj)
{
	//获得串口返回消息组合，根据串口类型分发

}

void MainServiceControll::SerialPortMessage(const QByteArray& code, const QVariant& message)
{

}

bool MainServiceControll::InitMission()
{
	//可以获得具体的任务路径
	QDir dir(this->mission.jobPath);
	if (!dir.exists()) {
		if (!dir.mkpath(this->mission.jobPath)) {
			this->CallErrorMessage("InitMission - createMissionPathFailed ");
			return false;
		}
	}

	if (!dir.cd("Task")) {
		if (!dir.mkpath("Task")) {
			this->CallErrorMessage("InitMissionPath - createMissionPathFailed");
			return false;
		}
		dir.cd("Task");
		this->strTaskPath = dir.absolutePath();
	}

	dir.setPath(this->mission.jobPath);

	//如果当前设备需要拍照的时候的时候，需要尝试创建一个Pic文件夹

	if (this->isPictureTaker()) {
		if (!dir.cd("Pics")) {
			if (!dir.mkpath("Pics")) {
				this->CallErrorMessage("InitMissionPath - createMissionPathFailed");
				return false;
			}
			dir.cd("Pics");
			this->strPicsPath = dir.absolutePath();
		}
	}

	return true;
}

bool MainServiceControll::isPictureTaker()
{
	if (this->param.type == MainServiceType::MS201 ||
		this->param.type == MainServiceType::ZS_Whole ||
		this->param.type == MainServiceType::ZS_Half ||
		this->param.type == MainServiceType::HS_VTM) {
		return true;
	}
	return false;
}

bool MainServiceControll::InitFile(const QString& filepath, QFile* file)
{

	if (file == nullptr) {
		file = new QFile(this);
	}
	if (file->isOpen()) {
		return true;
	}
	file->setFileName(filepath);
	if (file->exists()) {
		return true;
	}
	else {
		// 创建这个文件
		if (file->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append)) {
			//如果创建了文件之后，需要尝试在这个文件里写入初始化头
			QFileInfo info(*file);
			QString preffix = info.baseName();
			QString str_title = "";
			if (preffix.contains(centralPosition)) {
				str_title = "ID" + strTab +
					"CamID" + strTab +
					"CentralPosition" + strTab +
					"time" + strTab +
					"ActualPosition";
			}
			else if (preffix.contains(mileage)) {
				str_title = "ID" + strTab +
					"LeftMileage" + strTab +
					"LeftTime" + strTab +
					"LeftTimeRaw" + strTab +
					"RightMileage" + strTab +
					"RightTime" + strTab +
					"RightTimeRaw";
			}
			else if (preffix.contains(singleMileage)) {
				str_title = "ID" + strTab +
					"Mileage" + strTab +
					"Time" + strTab +
					"TimeRaw";
			}
			else if (preffix.contains(scannerTime)) {
				str_title = "ID" + strTab +
					"TrolleyTime" + strTab +
					"ScannerTime";
			}
			else if (preffix.contains(Inclinometer)) {
				str_title = "X" + strTab +
					"Y" + strTab +
					"Time" + strTab;
			}
			else if (preffix.contains(FaroFileInfo)) {
				str_title = "time" + strTab +
					"name";
			}
			return true;
		}
		else {
			this->CallErrorMessage("InitFile - Failed to create file");
			return false;
		}
	}
	return true;
}

void MainServiceControll::CameraTimeUpdateHandler(const QList<QVariant>& obj)
{
	if (obj.size() < 4) return;
	if (!this->InitFile(this->strPicsPath + QDir::separator() + centralPosition + ".txt", this->file_centralPosition)) {
		return;
	}

	QString insert = QString("%1\t%2\t%3\t%4")
		.arg(obj[0].toInt())
		.arg(obj[1].toInt())
		.arg(obj[2].toInt())
		.arg(obj[3].toLongLong());

	this->file_centralPosition->write(insert.toLocal8Bit());
}

void MainServiceControll::ScannerTimeUpdateHandler(const QList<QVariant>& obj)
{
	if (obj.size() < 2) return;
	if (!this->InitFile(this->strTaskPath + QDir::separator() + scannerTime + ".txt", this->file_scannerTime)) {
		return;
	}

	QString insert = QString("%1\t%2")
		.arg(obj[0].toLongLong())
		.arg(obj[1].toLongLong());
	this->file_scannerTime->write(insert.toLocal8Bit());
}

void MainServiceControll::TrolleyStatusHandler(const QList<QVariant>& obj)
{
	if (obj.size() < 3) return;
	QJsonObject ret;
	ret.insert("Temperature", obj.at(0).toInt());
	ret.insert("Direction", obj.at(1).toBool());
	ret.insert("BatteryUsage", obj.at(2).toInt());

	this->CallMainProcess(UpDateSPState, QJsonDocument(ret).toJson());
}

void MainServiceControll::BatteryStateHandler(const QList<QVariant>& obj)
{
	if (obj.size() < 3) return;
	QJsonObject ret;
	ret.insert("Left", obj.at(0).toDouble());
	ret.insert("Right", obj.at(1).toDouble());
	ret.insert("BatteryUsage", obj.at(2).toDouble());

	this->CallMainProcess(UpdateSPBatteryUsage, QJsonDocument(ret).toJson());
}

void MainServiceControll::SingleEncoderMileageUpdateHandler(const QList<QVariant>& obj)
{
	if (obj.size() < 4) return;
	if (!this->InitFile(this->strTaskPath + QDir::separator() + singleMileage + ".txt", this->file_singleMileage)) {
		return;
	}
	if (!this->InitFile(this->strTaskPath + QDir::separator() + mileage + ".txt", this->file_mileage)) {
		return;
	}
	qint32 ID = obj[0].toInt();
	qint32 Mileage = obj[1].toInt();
	qint64 Time = obj[2].toLongLong();
	qint64 TimeRaw = obj[3].toLongLong();

	QString insert = QString("%1\t%2\t%3\t%4")
		.arg(ID)
		.arg(Mileage)
		.arg(Time)
		.arg(TimeRaw);

	this->file_singleMileage->write(insert.toLocal8Bit());

	if (ID % 20 == 0) {
		QJsonObject ret;
		ret.insert("ID", ID);
		ret.insert("Mileage", Mileage);
		ret.insert("Time", Time);
		ret.insert("TimeRaw", TimeRaw);
		this->CallMainProcess(UpdateSPMileage, QJsonDocument(ret).toJson());
	}
}

void MainServiceControll::DoubleEncoderMileageUpdateHandler(const QList<QVariant>& obj)
{
	if (obj.size() < 7) return;
	if (!this->InitFile(this->strTaskPath + QDir::separator() + mileage + ".txt", this->file_mileage)) {
		return;
	}
	qint32 ID = obj[0].toInt();
	qint32 LeftMileage = obj[1].toInt();
	qint64 LeftTime = obj[2].toLongLong();
	qint64 LeftTimeRaw = obj[3].toLongLong();
	qint32 RightMileage = obj[4].toInt();
	qint64 RightTime = obj[5].toLongLong();
	qint64 RightTimeRaw = obj[6].toLongLong();

	QString insert = QString("%1\t%2\t%3\t%4\t%5\t%6\t%7")
		.arg(ID)
		.arg(LeftMileage)
		.arg(LeftTime)
		.arg(LeftTimeRaw)
		.arg(RightMileage)
		.arg(RightTime)
		.arg(RightTimeRaw);

	this->file_mileage->write(insert.toLocal8Bit());

	if (ID % 20 == 0) {
		QJsonObject ret;
		ret.insert("ID", ID);
		ret.insert("LeftMileage", LeftMileage);
		ret.insert("LeftTime", LeftTime);
		ret.insert("LeftTimeRaw", LeftTimeRaw);
		ret.insert("RightMileage", RightMileage);
		ret.insert("RightTime", RightTime);
		ret.insert("RightTimeRaw", RightTimeRaw);
		this->CallMainProcess(UpdateDoubleMileage, QJsonDocument(ret).toJson());
	}
}

void MainServiceControll::InclinometerMessageUpdateHandler(const QList<QVariant>& obj)
{
	if (obj.size() < 3) return;
	if (!this->InitFile(this->strTaskPath + QDir::separator() + Inclinometer + ".txt", this->file_Inclinometer)) {
		return;
	}
	qint32 X = obj[0].toInt();
	qint32 Y = obj[1].toInt();
	qint64 Time = obj[2].toLongLong();
	QString insert = QString("%1\t%2\t%3").arg(X).arg(Y).arg(Time);
	this->file_Inclinometer->write(insert.toLocal8Bit());
}

void MainServiceControll::IMUUpdateHandler(const QList<QVariant>& obj)
{
	if (obj.size() < 8) return;
	if (!this->InitFile(this->strTaskPath + QDir::separator() + IMU + ".txt", this->file_Imu)) {
		return;
	}
	QString XRotation = obj[0].toString();
	QString YRotation = obj[1].toString();
	QString ZRotation = obj[2].toString();
	QString XAcceleration = obj[3].toString();
	QString YAcceleration = obj[4].toString();
	QString ZAcceleration = obj[5].toString();
	qint32 time = obj[6].toInt();
	qint64 ID = obj[7].toLongLong();
	QString insert = QString("%1\t%2\t%3\t%4\t%5\t%6\t%7\t%8").arg(ID).arg(time).arg(XRotation).arg(YRotation).arg(ZRotation).arg(XAcceleration).arg(YAcceleration).arg(ZAcceleration);

	this->file_Imu->write(insert.toLocal8Bit());

}

void MainServiceControll::CallMainProcess(const QString& sDescribe, const QString& message)
{
	emit this->Sig_CallMethod(MainProcess, sDescribe, message, "");
}

void MainServiceControll::CallDeviceManager(const QString& sDescribe, const QString& message)
{
	emit this->Sig_CallMethod(DeviceManager, sDescribe, message, "");
}