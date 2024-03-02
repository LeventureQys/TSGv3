#include "MissionController.h"
MissionController::MissionController(QObject* parent) : TSG_Framework(parent, str_classname)
{
}
bool MissionController::Init(const QString& ApplicationPath)
{
	//中心服务器初始化尝试
	QDir dir(ApplicationPath);
	this->str_ApplicationPath = ApplicationPath;

	if (!dir.exists()) {
		this->str_ApplicationPath = defaultScanPath;
	}

	dir.setPath(this->str_ApplicationPath);

	if (!dir.exists()) {
		this->CallErrorMessage("Init - path not exist!");
		return false;
	}

	if (!dir.cd(TopMissionFolderName)) {
		//如果不存在Scan文件夹，则创建一个
		if (!dir.mkpath(TopMissionFolderName)) {
			this->CallErrorMessage("Init - Can't make a Scan folder");
			return false;
		}
		dir.cd(TopMissionFolderName);
	}

	//此时已经进入到Mission文件夹内了
	this->str_CurrentMissionPath = dir.absolutePath();
	this->getMissionList();
	return true;
}

QList<QString> MissionController::getMissionList()
{
	//在获得MissionList之前需要获得一遍所有的任务
	if (this->str_CurrentMissionPath.isEmpty()) {
		this->CallErrorMessage("getMissionList - CurrentMissionPath is empty!");
		return QList<QString>();
	};
	QDir dir(this->str_CurrentMissionPath);
	if (dir.isEmpty()) {
		this->CallErrorMessage("getMissionList - CurrentMissionPath is empty!");
		return QList<QString>();
	}

	//获得所有的项目
	QList<QString> list_Mission = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

	for (auto item : list_Mission) {
		QDir dir_mission(this->str_CurrentMissionPath + QDir::separator() + item);
		//检查dir_mission是否合法
		if (!dir_mission.exists()) {
			continue;
		}
		//还需要从任务路径下找到info.json文件，然后读取其中的内容，并填充到MissionContent中
		QFile file(dir_mission.absolutePath() + QDir::separator() + "info.json");
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			this->CallErrorMessage("getMissionList - Can't open a info.json");
			continue;
		}

		QByteArray data = file.readAll();

		QString str_json = QString::fromLocal8Bit(data);
		MissionContent content;
		if (!content.fromJson(str_json)) {
			this->CallErrorMessage("getMissionList - Can't parse a info.json" + str_json);
			continue;
		}

		this->map_MissionContent.insert(content.MissionName, content);
	}

	return this->map_MissionContent.keys();
}

bool MissionController::CreateMission(const QString& MissionName)
{
	//给定任务名称，创建一个默认的任务 
	MissionContent mission;
	mission.MissionName = MissionName;
	mission.CreateTime = QDateTime::currentDateTime();

	return this->CreateMission(mission.MissionName, mission);
}

bool MissionController::CreateMission(const QString& MissionName, const MissionContent& Content)
{
	//尝试在文件夹中创建一个这个文件夹，并且
	QDir dir(str_CurrentMissionPath);
	QString mission_name = MissionName + ".nfproj";
	if (!dir.cd(mission_name)) {
		if (!dir.mkdir(mission_name)) {
			this->CallErrorMessage("CreateMission - mkdir failed!" + mission_name);
			return false;
		}
		if (!dir.cd(mission_name)) {
			this->CallErrorMessage("CreateMission - can't enter the dir" + dir.absolutePath());
			return false;
		}
	}
	//创建目录至少是成功了的
	//尝试创建一个dir下的info.json文件
	QFile file(dir.absolutePath() + QDir::separator() + "info.json");
	if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
		this->CallErrorMessage("CreateMission - Can't Create File " + file.fileName());
		return false;
	}
	QString str_content = Content.toString();
	file.write(str_content.toLocal8Bit());
	file.flush();
	file.close();
	this->map_MissionContent.insert(MissionName, Content);
	this->getMissionList();
	return true;
}

bool MissionController::DeleteMission(const QString& MissionName)
{
	QDir dir(this->str_CurrentMissionPath);

	//找到目录下的Mission，并尝试删除
	QString mission_name = MissionName + ".nfproj";
	dir.cd(mission_name);

	if (!dir.exists()) {
		return true;
	}

	if (dir.removeRecursively()) {
		this->map_MissionContent.remove(MissionName);
		return true;
	}

	return false;
}

bool MissionController::setMissionValue(const QString& MissionName, const MissionContent& Content)
{
	QDir dir(this->str_CurrentMissionPath);
	auto content = this->map_MissionContent.find(MissionName);
	if (content == this->map_MissionContent.end()) {
		this->CallErrorMessage("setMissionValue - Cant find MissionName : " + MissionName);
		return false;
	}

	//尝试改变指定位置的文件夹下的info.json文件
	if (!dir.cd(MissionName)) {
		this->CallErrorMessage("setMissionValue - Can't cd MissionName" + MissionName);
		return false;
	}

	QFile file(dir.absolutePath() + QDir::separator() + "info.json");
	if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
		this->CallErrorMessage("CreateMission - Can't Create File " + file.fileName());
		return false;
	}
	QString str_content = Content.toString();
	file.write(str_content.toLocal8Bit());
	file.flush();
	file.close();
	this->map_MissionContent.insert(MissionName, Content);
	return true;
}

bool MissionController::StartMission(const QString& MissionName)
{
	//设置一个当前的任务
	this->currentMissionName = MissionName;

	MissionContent content = this->map_MissionContent[MissionName];
	content.LastWorkTime = QDateTime::currentDateTime();

	if (!this->setMissionValue(MissionName, content)) return false;
	emit Sig_StartMissionInfo(content);

	return true;
}

MissionContent MissionController::getCurrentMissionContent()
{
	if (!this->map_MissionContent.contains(this->currentMissionName)) {
		return MissionContent();
	}
	else {
		return this->map_MissionContent[this->currentMissionName];
	}
}
