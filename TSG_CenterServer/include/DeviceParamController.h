
#pragma once
#include "TSG_Framework.h"
#include "PublicVar.h"
#include "qdir.h"
/// <summary>
///设备参数控制类，一个设备类型中的层级应该是 项目名称-设备配置
/// </summary>
class DeviceParamController : public TSG_Framework {
public:
	DeviceParamController(QObject* parent);
	~DeviceParamController();

	/// <summary>
	/// 初始化设备参数
	/// </summary>
	/// <param name="ApplicationPath">应用程序路径</param>
	/// <returns>返回是否初始化成功
	/// returns>
	bool Init(const QString& ApplicationPath);

	/// <summary>
	/// 获得设备信息列表
	/// </summary>
	/// <returns>设备信息列表</returns>
	QList<QString> getDeviceParamList();
private:
	const QString str_classname = "DeviceParamController";
	QString str_ApplicationPath;
	QString str_CurrentDeviceParamPath;
	QMap<QString, DeviceParam> map_DeviceParam;
};