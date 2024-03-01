
#pragma once
#include "TSG_Framework.h"
#include "PublicVar.h"
/// <summary>
///设备参数控制
/// </summary>
class DeviceParamController : public TSG_Framework {
public:
	DeviceParamController(QObject* parent);
	~DeviceParamController();

	bool Init(const QString& ApplicationPath);

};