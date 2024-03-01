#include "DeviceParamController.h"

DeviceParamController::DeviceParamController(QObject* parent) : TSG_Framework(parent, str_classname)
{

}
DeviceParamController::~DeviceParamController()
{
}
bool DeviceParamController::Init(const QString& ApplicationPath)
{
	//设备信息初始化尝试
	QDir dir(ApplicationPath);
	this->str_ApplicationPath = ApplicationPath;

	if (!dir.exists()) {
		this->str_ApplicationPath = defaultDeviceParamPath;
	}

	dir.setPath(this->str_ApplicationPath);

	if (!dir.exists()) {
		this->CallErrorMessage("Init - path not exist!");
		return false;
	}

	if (!dir.cd(DeviceParamFolderName)) {
		//如果不存在Device文件夹，则创建一个
		if (!dir.mkpath(DeviceParamFolderName)) {
			this->CallErrorMessage("Init - Can't make a Scan folder");
			return false;
		}
		dir.cd(DeviceParamFolderName);
	}
	this->str_CurrentDeviceParamPath = dir.absolutePath();
	return true;
}

QList<QString> DeviceParamController::getDeviceParamList()
{
	//在获得MissionList之前需要获得一遍所有的任务
	if (this->str_CurrentDeviceParamPath.isEmpty()) {
		this->CallErrorMessage("getMissionList - CurrentMissionPath is empty!");
		return QList<QString>();
	};
	QDir dir(this->str_CurrentDeviceParamPath);
	if (dir.isEmpty()) {
		this->CallErrorMessage("getMissionList - CurrentMissionPath is empty!");
		return QList<QString>();
	}

	//获得所有的项目
	QList<QString> list_DeviceParams = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
	for (auto item : list_DeviceParams) {

		QDir dir_deviceParams(this->str_CurrentDeviceParamPath + QDir::separator() + item);
		//检查dir_deviceParams是否合法
		if (!dir_deviceParams.exists()) {
			continue;
		}
		//根据实际参数的不同，对各个设备写入一系列参数
		QFile file(dir_deviceParams.absolutePath() + QDir::separator() + "DeviceParam.json");
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			this->CallErrorMessage("getMissionList - Can't open a info.json");
			continue;
		}
		QByteArray data = file.readAll();

		QString str_json = QString::fromLocal8Bit(data);
		DeviceParam content;
		if (!content.fromJson(str_json)) {
			this->CallErrorMessage("getMissionList - Can't parse a info.json" + str_json);
			continue;
		}
		this->map_DeviceParam.insert(content.DeviceParamsName, content);
	}
}