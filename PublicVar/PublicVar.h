#pragma once
#include "qstring.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qjsondocument.h"

/// <summary>
/// 默认文件存储路径
/// </summary>
const QString defaultScanPath = "D://Scan";
const QString defaultDeviceParamPath = "D://Device";
const QString Trolley = "Trolley";
const QString CentralControl = "CentralControl";

const QString English = "English";
const QString Chinese = "Chinese";

const QString DefaultIP = "192.200.1.20";
const QString DefaultPort = "8000";
const QString DefaultServiceType = "CentralControl";

const QString DeviceName = "DeviceName";
const QString Language = "Language";
const QString IP = "IP";
const QString Port = "Port";
const QString ServiceType = "ServiceType";

const QString Normal = "Normal";
const QString Error = "Error";

const QString TSG_Dialog = "TSG_Dialog";
const QString TopMissionFolderName = "Mission";
const QString DeviceParamFolderName = "Device";

const QString SerialPort = "SerialPort";
//设备类型
enum class ServerType {
	MainService,
	Device,
	GUI
};

//认证信息
struct AuthorizeInfo {
	ServerType type = ServerType::MainService;
	QString ip = "";
	qint16 port = 0;	//端口一般情况下不重要
	QString DeviceName = "";
	QString toString() {
		QString type_ = "";
		if (this->type == ServerType::MainService) {
			type_ = "MainService";
		}
		else if (this->type == ServerType::Device) {
			type_ = "Device";
		}
		else if (this->type == ServerType::GUI) {
			type_ = "GUI";
		}

		QJsonObject obj;
		obj.insert("InfoType", "Authorize");
		QJsonObject obj_Content;

		obj_Content.insert("type", type_);
		obj_Content.insert("ip", this->ip);
		obj_Content.insert("port", this->port);
		obj_Content.insert("DeviceName", this->DeviceName);

		obj.insert("Content", obj_Content);
		return QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
	}

	void AnalyzeFromString(QString info) {
		QJsonDocument doc = QJsonDocument::fromJson(info.toUtf8());
		QJsonObject obj = doc.object();
		QString type_ = obj.value("type").toString();
		if (type_ == "MainService") {
			this->type = ServerType::MainService;
		}
		else if (type_ == "Device") {
			this->type = ServerType::Device;
		}
		else if (type_ == "GUI") {
			this->type = ServerType::GUI;
		}
		this->ip = obj.value("ip").toString();
		this->port = obj.value("port").toInt();
		this->DeviceName = obj.value("DeviceName").toString();
	}
};



struct TrolleyParam {
	bool blnDirection = true;		//true:正向，false:反向
	qint32 speed = 1500;
	qint32 nAnalyseMode = 0;		//0标准模式，1无处理模式

	QString toString() {
		QJsonObject obj;
		obj.insert("blnDirection", blnDirection);
		obj.insert("speed", speed);
		obj.insert("nAnalyseMode", nAnalyseMode);
		return QJsonDocument(obj).toJson();
	}

	void FromString(const QString& strMessage) {
		QJsonObject obj = QJsonDocument::fromJson(strMessage.toUtf8()).object();
		blnDirection = obj.value("blnDirection").toBool();
		speed = obj.value("speed").toInt();
		nAnalyseMode = obj.value("nAnalyseMode").toInt();

	}
};

struct CameraParam {
	float Fps = 1.00;	//帧率
	float Gain = 8.00;	//增益
	float Gamma = 0.00;//伽马
	qint32 Brightness = 100;//亮度
	qint32 BlackLevel = 110;//黑电平
	qint32 BinningHorizontal = 1;//水平像素合并
	qint32 Sharpness = 10;//锐度
	QString saveFormat = "JPG";	//默认保存JPEG格式
	float ExposureTime = 600.00;//曝光时间
	QString toString() {
		QJsonObject obj;
		obj.insert("Fps", Fps);
		obj.insert("Gain", Gain);
		obj.insert("Gamma", Gamma);
		obj.insert("Brightness", Brightness);
		obj.insert("BlackLevel", BlackLevel);
		obj.insert("BinningHorizontal", BinningHorizontal);
		obj.insert("Sharpness", Sharpness);
		obj.insert("saveFormat", saveFormat);
		obj.insert("ExposureTime", ExposureTime);
		return QJsonDocument(obj).toJson();
	}
	void FromString(const QString& strMessage) {
		QJsonObject obj = QJsonDocument::fromJson(strMessage.toUtf8()).object();
		Fps = obj.value("Fps").toDouble();
		Gain = obj.value("Gain").toDouble();
		Gamma = obj.value("Gamma").toDouble();
		Brightness = obj.value("Brightness").toInt();
		BlackLevel = obj.value("BlackLevel").toInt();
		BinningHorizontal = obj.value("BinningHorizontal").toInt();
		Sharpness = obj.value("Sharpness").toInt();
		saveFormat = obj.value("saveFormat").toString();
		ExposureTime = obj.value("ExposureTime").toDouble();
	}
};
struct ScannerParam {
	int resolution = 4;
	int noiseCompression = 1;
	int measurementRate = 8;
	int vertAngleMin = -60;
	int vertAngleMax = 90;
	int splitAfterLines = 5000;
	int numCols = 2000000;
	/// <summary>
	/// 连接的认证信息
	/// </summary>
	QString str_connectType = "LAN";
	QString str_serialNumber = "";
	QString str_scanKey = "";
	QString str_scanIP = "";
	QString toString() {
		QJsonObject obj;
		obj.insert("resolution", resolution);
		obj.insert("noiseCompression", noiseCompression);
		obj.insert("measurementRate", measurementRate);
		obj.insert("vertAngleMin", vertAngleMin);
		obj.insert("vertAngleMax", vertAngleMax);
		obj.insert("splitAfterLines", splitAfterLines);
		obj.insert("numCols", numCols);
		obj.insert("str_connectType", str_connectType);
		obj.insert("str_serialNumber", str_serialNumber);
		obj.insert("str_scanKey", str_scanKey);
		obj.insert("str_scanIP", str_scanIP);
		return QJsonDocument(obj).toJson();
	}
	void FromString(const QString& strMessage) {
		QJsonObject obj = QJsonDocument::fromJson(strMessage.toUtf8()).object();
		resolution = obj.value("resolution").toInt();
		noiseCompression = obj.value("noiseCompression").toInt();
		measurementRate = obj.value("measurementRate").toInt();
		vertAngleMin = obj.value("vertAngleMin").toInt();
		vertAngleMax = obj.value("vertAngleMax").toInt();
		splitAfterLines = obj.value("splitAfterLines").toInt();
		numCols = obj.value("numCols").toInt();
		str_connectType = obj.value("str_connectType").toString();
		str_serialNumber = obj.value("str_serialNumber").toString();
		str_scanKey = obj.value("str_scanKey").toString();
		str_scanIP = obj.value("str_scanIP").toString();
	}
};

struct DeviceParam {
	QString DeviceParamsName = "";
	QString Creator = "";
	QString Note = "";
	TrolleyParam trolleyParam;
	CameraParam cameraParam;
	ScannerParam scannerParam;

	//决定设备是否开启的配置项
	bool blnTrolley = true;
	bool blnCamera = true;
	bool blnScanner = true;

	QString toString() const {
		QJsonObject obj;
		obj.insert("InfoType", "DeviceParam");
		obj.insert("DeviceParamsName", DeviceParamsName);
		obj.insert("Creator", Creator);
		obj.insert("Note", Note);
		obj.insert("blnTrolley", blnTrolley);
		obj.insert("blnCamera", blnCamera);
		obj.insert("blnScanner", blnScanner);
		QJsonObject obj_Trolley;
		obj_Trolley.insert("blnDirection", trolleyParam.blnDirection);
		obj_Trolley.insert("speed", trolleyParam.speed);
		obj_Trolley.insert("nAnalyseMode", trolleyParam.nAnalyseMode);
		QJsonObject obj_Camera;
		obj_Camera.insert("Fps", cameraParam.Fps);
		obj_Camera.insert("Gain", cameraParam.Gain);
		obj_Camera.insert("Gamma", cameraParam.Gamma);
		obj_Camera.insert("Brightness", cameraParam.Brightness);
		obj_Camera.insert("BlackLevel", cameraParam.BlackLevel);
		obj_Camera.insert("BinningHorizontal", cameraParam.BinningHorizontal);
		obj_Camera.insert("Sharpness", cameraParam.Sharpness);
		obj_Camera.insert("saveFormat", cameraParam.saveFormat);
		obj_Camera.insert("ExposureTime", cameraParam.ExposureTime);
		QJsonObject obj_Scanner;
		obj_Scanner.insert("resolution", scannerParam.resolution);
		obj_Scanner.insert("noiseCompression", scannerParam.noiseCompression);
		obj_Scanner.insert("measurementRate", scannerParam.measurementRate);
		obj_Scanner.insert("vertAngleMin", scannerParam.vertAngleMin);
		obj_Scanner.insert("vertAngleMax", scannerParam.vertAngleMax);
		obj_Scanner.insert("splitAfterLines", scannerParam.splitAfterLines);
		obj_Scanner.insert("numCols", scannerParam.numCols);
		obj.insert("trolleyParam", obj_Trolley);
		obj.insert("cameraParam", obj_Camera);
		obj.insert("scannerParam", obj_Scanner);
		return QJsonDocument(obj).toJson();
	}

	bool fromJson(const QString& string) {
		// 将 JSON 字符串解析为 QJsonDocument 对象
		QJsonDocument jsonDoc = QJsonDocument::fromJson(string.toUtf8());
		if (jsonDoc.isNull()) {
			qWarning() << "Failed to parse JSON document.";
			return false;
		}

		// 检查 JSON 对象是否符合预期的结构
		if (!jsonDoc.isObject()) {
			qWarning() << "JSON document is not an object.";
			return false;
		}

		// 获取根 JSON 对象
		QJsonObject obj = jsonDoc.object();
		if (!obj.contains("InfoType") || obj["InfoType"].toString() != "DeviceParam") {
			qWarning() << "JSON object is not of type 'MissionContent'.";
			return false;
		}


		qDebug() << obj;
		DeviceParamsName = obj.value("DeviceParamsName").toString();
		Creator = obj.value("Creator").toString();
		Note = obj.value("Note").toString();
		blnTrolley = obj.value("blnTrolley").toBool();
		blnCamera = obj.value("blnCamera").toBool();
		blnScanner = obj.value("blnScanner").toBool();
		QJsonObject obj_Trolley = obj.value("trolleyParam").toObject();
		trolleyParam.blnDirection = obj_Trolley.value("blnDirection").toBool();
		trolleyParam.speed = obj_Trolley.value("speed").toInt();
		trolleyParam.nAnalyseMode = obj_Trolley.value("nAnalyseMode").toInt();

		QJsonObject obj_Camera = obj.value("cameraParam").toObject();
		cameraParam.Fps = obj_Camera.value("Fps").toDouble();
		cameraParam.Gain = obj_Camera.value("Gain").toDouble();
		cameraParam.Gamma = obj_Camera.value("Gamma").toDouble();
		cameraParam.Brightness = obj_Camera.value("Brightness").toInt();
		cameraParam.BlackLevel = obj_Camera.value("BlackLevel").toInt();
		cameraParam.BinningHorizontal = obj_Camera.value("BinningHorizontal").toInt();
		cameraParam.Sharpness = obj_Camera.value("Sharpness").toInt();
		cameraParam.saveFormat = obj_Camera.value("saveFormat").toString();
		cameraParam.ExposureTime = obj_Camera.value("ExposureTime").toDouble();

		QJsonObject obj_Scanner = obj.value("scannerParam").toObject();
		scannerParam.resolution = obj_Scanner.value("resolution").toInt();
		scannerParam.noiseCompression = obj_Scanner.value("noiseCompression").toInt();
		scannerParam.measurementRate = obj_Scanner.value("measurementRate").toInt();
		scannerParam.vertAngleMin = obj_Scanner.value("vertAngleMin").toInt();
		scannerParam.vertAngleMax = obj_Scanner.value("vertAngleMax").toInt();
		scannerParam.splitAfterLines = obj_Scanner.value("splitAfterLines").toInt();
		scannerParam.numCols = obj_Scanner.value("numCols").toInt();
		return true;
	}

};

//任务内容
struct MissionContent {
	QString MissionName = "";		//任务名称
	double Diameter = 5.4;			//隧道直径
	qint32 OverlapRate = 20;		//重叠率
	QString note;					//备注
	qint32 jobCount = 0;			//作业次数
	QDateTime CreateTime;			//创建时间
	QDateTime LastWorkTime;			//最后作业时间
	DeviceParam DeviceParam;		//设备详细信息
	QString toString() const {
		QJsonObject obj;
		obj.insert("InfoType", "MissionContent");
		QJsonObject obj_Content;

		obj_Content.insert("Diameter", this->Diameter);
		obj_Content.insert("OverlapRate", this->OverlapRate);
		obj_Content.insert("note", this->note);
		obj_Content.insert("jobCount", this->jobCount);
		obj_Content.insert("CreateTime", this->CreateTime.toString("yyyy-MM-dd hh:mm:ss"));
		obj_Content.insert("LastWorkTime", this->LastWorkTime.toString("yyyy-MM-dd hh:mm:ss"));
		obj_Content.insert("DeviceParam", DeviceParam.toString());
		obj.insert("Content", obj_Content);
		return QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
	}

	bool fromJson(const QString& string) {
		// 将 JSON 字符串解析为 QJsonDocument 对象
		QJsonDocument jsonDoc = QJsonDocument::fromJson(string.toUtf8());
		if (jsonDoc.isNull()) {
			qWarning() << "Failed to parse JSON document.";
			return false;
		}

		// 检查 JSON 对象是否符合预期的结构
		if (!jsonDoc.isObject()) {
			qWarning() << "JSON document is not an object.";
			return false;
		}

		// 获取根 JSON 对象
		QJsonObject jsonObj = jsonDoc.object();
		if (!jsonObj.contains("InfoType") || jsonObj["InfoType"].toString() != "MissionContent") {
			qWarning() << "JSON object is not of type 'MissionContent'.";
			return false;
		}

		// 获取内容对象
		QJsonObject contentObj = jsonObj["Content"].toObject();
		if (contentObj.contains("MissionName")) {
			MissionName = contentObj["MissionName"].toString();
		}
		// 从内容对象中读取数据并赋值给结构体成员
		if (contentObj.contains("Diameter")) {
			Diameter = contentObj["Diameter"].toDouble();
		}
		if (contentObj.contains("OverlapRate")) {
			OverlapRate = contentObj["OverlapRate"].toInt();
		}
		if (contentObj.contains("note")) {
			note = contentObj["note"].toString();
		}
		if (contentObj.contains("jobCount")) {
			jobCount = contentObj["jobCount"].toInt();
		}
		if (contentObj.contains("CreateTime")) {
			CreateTime = QDateTime::fromString(contentObj["CreateTime"].toString(), "yyyy-MM-dd hh:mm:ss");
		}
		if (contentObj.contains("LastWorkTime")) {
			LastWorkTime = QDateTime::fromString(contentObj["LastWorkTime"].toString(), "yyyy-MM-dd hh:mm:ss");
		}

		if (contentObj.contains("DeviceParam")) {
			DeviceParam.fromJson(contentObj["DeviceParam"].toString());
		}
		return true;
	}
};