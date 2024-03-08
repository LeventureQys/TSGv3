#include "FaroSDK.h"

FaroController::FaroController(QObject* obj) : TSG_Framework(obj, str_classname)
{

}

FaroController::~FaroController()
{

}

bool FaroController::InitConnection(const MissionContent& mission)
{
	this->Status = FaroStatus::DEFAULT;
	this->Status = FaroStatus::INITFAROING;


	this->mission = mission;
	this->param = mission.DeviceParam.scannerParam;
	if (!TryPingDevice(this->param)) return false;

	if (!InitHandler(this->param)) return false;




}
bool FaroController::TryPingDevice(const ScannerParam& InitParam)
{
	this->Status = FaroStatus::PINGING;
	QString address = InitParam.str_scanIP;
	QString program = "ping";
	QStringList arguments;
	arguments << address;

	QProcess pingProcess;
	pingProcess.start(program, arguments);
	pingProcess.waitForFinished(5000); // 最多ping五秒进程执行完毕

	QString pingResult = QString::fromLocal8Bit(pingProcess.readAll());

	//这里不知道为什么，正则表达式总是不生效
	if (pingResult.contains("TTL")) {
		this->Status = FaroStatus::PING;
		return true;
	}

	return false;
}
bool FaroController::InitHandler(const ScannerParam& InitParam)
{
	if (this->Status < FaroStatus::PING)
	{
		this->CallErrorMessage("InitHandler - Can't go through ping section");
		return false;
	}
	this->Status = FaroStatus::INITSDKING;

	if (InitParam.str_connectType.isEmpty() || InitParam.str_scanIP.isEmpty() || InitParam.str_scanKey.isEmpty() || InitParam.str_serialNumber.isEmpty())
	{
		this->CallErrorMessage(QString("InitHandler - InitParam is empty").arg(InitParam.str_scanKey));
		return false;
	}

	CoInitialize(NULL);

	BSTR licenseCode(_bstr_t(
		QString("FARO Open Runtime License\n"
			"Key:%1\n"
			"\n"
			"The software is the registered property of "
			"FARO Scanner Production GmbH, Stuttgart, Germany.\n"
			"All rights reserved.\n"
			"This software may only be used with written permission "
			"of FARO Scanner Production GmbH, Stuttgart, Germany.").arg(InitParam.str_scanKey).toStdString().c_str()));

	liPtr = IiQLicensedInterfaceIfPtr(__uuidof(ScanCtrlSDK));

	try
	{
		liPtr->License = licenseCode;
		scanCtrl = static_cast<IScanCtrlSDKPtr>(liPtr);
		this->CallNormalMessage("InitHandler - ServiceInitSuccess");
		this->Status = FaroStatus::INITSDK;
	}
	catch (...)
	{
		this->ResetHandler();
		this->CallErrorMessage("InitHandler - ServiceInitFailed");
		return false;
	}
	return true;
}

bool FaroController::ResetHandler()
{
	if (liPtr != nullptr) {
		liPtr->Release();
		liPtr = nullptr;
	}
	if (scanCtrl != nullptr) {
		scanCtrl->Release();
		scanCtrl = nullptr;
	}

	this->Status = FaroStatus::DEFAULT;

	return true;
}

bool FaroController::ConnectFaro()
{
	if (this->liPtr == nullptr || this->scanCtrl == nullptr)
	{
		this->CallErrorMessage("ConnectFaro - liPtr or scanCtrl is nullptr");
		return false;
	}
	if (this->Status < FaroStatus::INITSDK)
	{
		this->CallErrorMessage("ConnectFaro -Not INITSDK Yet");
		return false;
	}
	if (this->scanCtrl->Connected) {
		this->Status = FaroStatus::CONNECT;
		return true;
	}

	this->scanCtrl->ScannerIP = this->param.str_scanIP.toStdString().c_str();
	this->scanCtrl->clearExceptions();
	qint32 ret = this->scanCtrl->connect();
	qint32 count = 0;
	qint32 int_reconnect_count = 0; //重连计数器
	while (1) {
		count++;
		if (count > 44) {
			if (int_reconnect_count > 5) return false;
			//45秒后重连一次
			this->scanCtrl->clearExceptions();
			qint32 ret = this->scanCtrl->connect();
			int_reconnect_count++;
			count = 0;
		}
		if (scanCtrl->Connected || Status == FaroStatus::CONNECT) {
			return true;
		}
		qDebug() << "连接中：" << count;
		QThread::sleep(1);
	}
	return false;
}
BSTR fromQString(const QString& string) {
	return SysAllocString(reinterpret_cast<const OLECHAR*>(string.utf16()));
}

QString fromBSTR(BSTR string) {
	return QString::fromUtf16(reinterpret_cast<const ushort*>(string));
}
bool FaroController::setDeviceParam(const ScannerParam& param)
{
	if (this->Status < FaroStatus::CONNECT)
	{
		this->CallErrorMessage("setDeviceParam - Not CONNECT Yet");
		return false;
	}

	if (this->scanCtrl == nullptr) {
		this->CallErrorMessage("setDeviceParam - scanCtrl is nullptr");
		return false;
	}
	if (!this->InitMission(this->mission)) return false;
	this->scanCtrl->PutScanMode(ScanMode::HelicalCanGrey);
	this->scanCtrl->PutStorageMode(StorageMode::SMRemote);
	this->scanCtrl->PutScanFileNumber(this->param.scanFileNum);
	BSTR scanBaseName = fromQString(this->param.scanBaseName);
	this->scanCtrl->PutScanBaseName(scanBaseName);
	BSTR StoragePath = fromQString(this->str_pointcloudPath);
	this->scanCtrl->PutRemoteScanStoragePath(StoragePath);
	this->scanCtrl->PutResolution(this->param.resolution);
	this->scanCtrl->PutMeasurementRate(this->param.measurementRate);
	this->scanCtrl->PutNoiseCompression(this->param.noiseCompression);
	this->scanCtrl->PutVerticalAngleMin(this->param.vertAngleMin);
	this->scanCtrl->PutVerticalAngleMax(this->param.vertAngleMax);
	this->scanCtrl->PutNumCols(this->param.numCols);
	this->scanCtrl->PutSplitAfterLines(this->param.splitAfterLines);
	//保存
	int ret = this->scanCtrl->syncParam();

	QThread::sleep(1);
	if (!ret) {
		return true;
	}
	return false;
}

bool FaroController::InitMission(const MissionContent& mission)
{
	if (mission.jobPath.isEmpty()) return false;

	QDir dir(mission.jobPath);
	if (!dir.exists()) {
		if (dir.mkpath(mission.jobPath)) {
			dir.setPath(mission.jobPath);
		}
		else {
			return false;
		}
	}

	QString temp_pointcloudPath = mission.jobPath + QDir::separator() + "PointCloud";
	if (!dir.cd("PointCloud")) {
		if (dir.mkpath(temp_pointcloudPath)) {
			dir.setPath(temp_pointcloudPath);
		}
		else {
			return false;
		}
	}
	this->str_pointcloudPath = temp_pointcloudPath;
}

bool FaroController::StartScanRotation()
{
	if (this->Status < FaroStatus::CONNECT)
	{
		this->CallErrorMessage("StartScanRotation - Not CONNECT Yet");
		return false;
	}
	if (this->scanCtrl == nullptr) {
		this->CallErrorMessage("StartScanRotation - scanCtrl is nullptr");
		return false;
	}

	this->Status = FaroStatus::RUNCAMING;
	qint32 ret = this->scanCtrl->startScan();
	qint32 count = 0;
	qint32 int_reconnect_count = 0; //重连计数器
	while (1) {
		count++;
		if (count > 44) {
			if (int_reconnect_count > 5) return false;
			//45秒后重连一次
			this->scanCtrl->clearExceptions();
			qint32 ret = this->scanCtrl->startScan();
			int_reconnect_count++;
			count = 0;
		}
		this->scanCtrl->inquireRecordingStatus(&this->currentRecordingState);
		if (this->currentRecordingState == HelicalRecordingStatus::HRSPaused) {
			this->scanCtrl->pauseScan();
			this->Status = FaroStatus::RUNCAM;
			return true;
		}
		qDebug() << "开始旋转：" << count << "s";
		QThread::sleep(1);
	}
	return false;
}

bool FaroController::StartRecording()
{
	if (this->Status < FaroStatus::RUNCAM)
	{
		this->CallErrorMessage("StartRecording - Not RUNCAM Yet");
		return false;
	}
	if (this->scanCtrl == nullptr) {
		this->CallErrorMessage("StartRecording - scanCtrl is nullptr");
		return false;
	}
	this->Status = FaroStatus::RECORDFAROING;
	qint32 ret = this->scanCtrl->recordScan();
	qint32 count = 0;
	qint32 int_reconnect_count = 0; //重连计数器
	while (1) {
		count++;
		if (count > 44) {
			if (int_reconnect_count > 5) return false;
			//45秒后重连一次
			this->scanCtrl->clearExceptions();
			qint32 ret = this->scanCtrl->recordScan();
			int_reconnect_count++;
			count = 0;
		}
		this->scanCtrl->inquireRecordingStatus(&this->currentRecordingState);
		if (this->currentRecordingState == HelicalRecordingStatus::HRSRecording) {
			this->Status = FaroStatus::RECORDFARO;
			//此处法如正式开始采集，向上层返回开始采集成功

			return true;
		}
		qDebug() << "采集状态中：" << count << "s";
		QThread::sleep(1);
	}
	return false;
}

bool FaroController::PauseRecording()
{
	if (this->Status < FaroStatus::RECORDFARO)
	{
		this->CallErrorMessage("PauseRecording - Not RECORDFARO Yet");
		return false;
	}
	if (this->scanCtrl == nullptr) {
		this->CallErrorMessage("PauseRecording - scanCtrl is nullptr");
		return false;
	}
	this->Status = FaroStatus::PAUSEFARPING;
	qint32 count = 0;
	qint32 int_reconnect_count = 0; //重连计数器
	while (1) {
		count++;
		if (count > 44) {
			if (int_reconnect_count > 5) return false;
			//45秒后重连一次
			this->scanCtrl->clearExceptions();
			qint32 ret = this->scanCtrl->stopScan();
			int_reconnect_count++;
			count = 0;
		}
		this->scanCtrl->inquireRecordingStatus(&this->currentRecordingState);
		if (this->currentRecordingState == HelicalRecordingStatus::HRSPaused) {
			this->Status = FaroStatus::RECORDFARO;
			//此处法如正式开始采集，向上层返回开始采集成功
			return true;
		}
		qDebug() << "采集状态中：" << count << "s";
		QThread::sleep(1);
	}
	return false;
}

bool FaroController::StopRecording()
{
	if (this->Status < FaroStatus::RECORDFARO)
	{
		this->CallErrorMessage("StopRecording - Not RECORDFARO Yet");
		return false;
	}
	if (this->scanCtrl == nullptr) {
		this->CallErrorMessage("StopRecording - scanCtrl is nullptr");
		return false;
	}

	this->Status = FaroStatus::STOPFAROING;
	qint32 count = 0;
	qint32 int_reconnect_count = 0; //重连计数器
	while (1) {
		count++;
		if (count > 44) {
			if (int_reconnect_count > 5) return false;
			//45秒后重连一次
			this->scanCtrl->clearExceptions();
			qint32 ret = this->scanCtrl->stopScan();
			int_reconnect_count++;
			count = 0;
		}
		this->scanCtrl->inquireRecordingStatus(&this->currentRecordingState);
		if (this->currentRecordingState == HelicalRecordingStatus::HRSPaused) {
			this->Status = FaroStatus::STOPFARO;
			//此处法如结束采集
			this->Status = FaroStatus::CONNECT;
			return true;
		}
		qDebug() << "结束采集状态中：" << count << "s";
		QThread::sleep(1);
	}
	return true;
}

qint32 FaroController::checkScanProgress()
{
	if (this->scanCtrl != nullptr)
		return this->scanCtrl->ScanProgress;
	else
		return -1;
}

bool FaroController::StartMission()
{
	if (!this->StartScanRotation()) return false;
	if (!this->StartRecording()) {
		this->StopRecording();
		return false;
	}
	return true;
}

bool FaroController::PauseMission()
{
	return this->PauseRecording();
}

bool FaroController::EndMission()
{
	return this->StopRecording();
}