#pragma once
#include "qobject.h"
#include "PublicVar.h"
/// <summary>
/// 基础通信框架
/// </summary>
class TSG_Framework : public QObject {
	Q_OBJECT
public:
	TSG_Framework(QObject* parent, QString frameName) { this->str_framework = frameName; }
	~TSG_Framework() {}

	void CallNormalMessage(const QString& NormalMessage) {
		emit this->Sig_CallMethod(TSG_Dialog, Normal, NormalMessage, this->str_framework);
	}
	void CallErrorMessage(const QString& ErrorMessage) {
		emit this->Sig_CallMethod(TSG_Dialog, Error, ErrorMessage, this->str_framework);
	}
	void CallTypeMessage(const QString& TypeMessage, const QString& type) {
		emit this->Sig_CallMethod(TSG_Dialog, type, TypeMessage, this->str_framework);
	}
#pragma region 通信框架
public slots:
	virtual qint32 slot_ReturnValue(const QString& sModule, const QString& sDescribe, const QString& sVariable, const QVariant& extra) { return -1; };
	virtual qint32 slot_GetCMD(const QString& sModule, const QString& sDescribe, const QString& sVariable, const QVariant& extra) { return -1; };
signals:
	void Sig_CallMethod(const QString& sModule, const QString& sDescribe, const QString& sVariable, const QVariant& extra);
	void Sig_SendCMD(const QString& sModule, const QString& sDescribe, const QString& sVariable, const QVariant& extra);
#pragma endregion
private:
	QString str_framework;
};

/// <summary>
/// TSG_Device的状态机，用于表示当前状态
/// </summary>
enum class DeviceState {
	UnInit,
	Error,
	Init,
	Setting,
	Running,
	Pause,
	End,
	Analyse
};

/// <summary>
/// 当前设备类型，用于标识设备类型
/// </summary>
enum class DeviceType {
	SerialPort,
	HikCamera,
	FaroScanner,
};

/// <summary>
/// 单独的设备管理模块，需要一些基础的功能
/// </summary>
template<class InitParam, class MissionParams>
class TSG_Device {
public:
	TSG_Device(const QString str_classname) {}
	~TSG_Device() {  };
#pragma region 基础接口
	/// <summary>
/// 初始化设备对象，有的设备可能需要手动输入一个初始化参数，但是一般的设备不需要，看情况而定
/// </summary>
/// <param name="param">初始化参数</param>
/// <returns>初始化是否成功</returns>
	virtual bool Init(InitParam param) = 0;

	/// <summary>
	/// 设置任务参数
	/// </summary>
	/// <param name="param">任务参数</param>
	/// <returns>设置任务是否成功</returns>
	virtual bool SettingMission(MissionParams param) = 0;

	/// <summary>
	/// 预热机器，如果机器需要预热，则需要实现这个函数
	/// </summary>
	/// <returns>预热工作是否成功</returns>
	virtual bool PrewarmMachine() = 0;

	/// <summary>
	/// 正式开始任务，正式地开始采集
	/// </summary>
	/// <returns>开始任务是否成功</returns>
	virtual bool StartMission() = 0;

	/// <summary>
	/// 暂停任务
	/// </summary>
	/// <returns>暂停结果</returns>
	virtual bool PauseMission() = 0;

	/// <summary>
	/// 停止任务
	/// </summary>
	/// <returns>停止任务结果</returns>
	virtual bool EndMission() = 0;

	/// <summary>
	/// 停止任务之后的一个阶段，用于处理结果
	/// </summary>
	/// <returns></returns>
	virtual bool AnalyseResult() = 0;
#pragma endregion

#pragma region 设备状态
	/// <summary>
	/// 获得设备类型名称
	/// </summary>
	/// <returns></returns>
	virtual QString getDeviceType() = 0;
	/// <summary>
	/// 获得当前设备的状态机
	/// </summary>
	/// <returns></returns>
	virtual DeviceState getDevice() = 0;

	/// <summary>
	/// 获得初始化参数
	/// </summary>
	/// <returns>返回初始化参数</returns>
	virtual InitParam getInitParam() = 0;

	/// <summary>
	/// 获得当前任务参数
	/// </summary>
	/// <returns></returns>
	virtual MissionParams getMissinParams() = 0;
#pragma endregion


};