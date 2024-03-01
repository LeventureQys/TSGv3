#pragma once
#include "qstring.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qjsondocument.h"

/// <summary>
/// 默认文件存储路径
/// </summary>
const QString defaultScanPath = "D://Scan";

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
//设备类型
enum class DeviceType {
	MainService,
	Device,
	GUI
};

//认证信息
struct AuthorizeInfo {
	DeviceType type = DeviceType::MainService;
	QString ip = "";
	qint16 port = 0;	//端口一般情况下不重要
	QString DeviceName = "";
	QString toString() {
		QString type_ = "";
		if (this->type == DeviceType::MainService) {
			type_ = "MainService";
		}
		else if (this->type == DeviceType::Device) {
			type_ = "Device";
		}
		else if (this->type == DeviceType::GUI) {
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
			this->type = DeviceType::MainService;
		}
		else if (type_ == "Device") {
			this->type = DeviceType::Device;
		}
		else if (type_ == "GUI") {
			this->type = DeviceType::GUI;
		}
		this->ip = obj.value("ip").toString();
		this->port = obj.value("port").toInt();
		this->DeviceName = obj.value("DeviceName").toString();
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

		return true;
	}
};

struct DeviceBase {
	QString DeviceParamsName = "";
	QString Creator = "";
	QString Note = "";
	virtual QString toString() = 0;
	virtual void fromJson(const QString& json) = 0;
};

struct ParamTrolley : DeviceBase {
	bool blnDirection = true;		//true:正向，false:反向
	qint32 speed = 1500;
	qint32 nAnalyseMode = 0;		//暂时没有作用
	QString toString() {
		QJsonObject obj;
		obj.insert("DeviceParamsName", DeviceParamsName);
		obj.insert("Creator", Creator);
	}
};