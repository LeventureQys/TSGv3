#include "FaroSDK.h"

class FaroParam {
public:
	QList<ScannerParam> list_faros;
};

class FaroControl : public TSG_Device<FaroParam,MissionContent> {

	Q_OBJECT
public:
	FaroControl(QObject* parent, const QString& frameName);
	~FaroControl();

	/// <summary>
/// 初始化设备对象，有的设备可能需要手动输入一个初始化参数，但是一般的设备不需要，看情况而定
/// </summary>
/// <param name="param">初始化参数</param>
/// <returns>初始化是否成功</returns>
	 bool Init(FaroParam param);

	/// <summary>
	/// 设置任务参数
	/// </summary>
	/// <param name="param">任务参数</param>
	/// <returns>设置任务是否成功</returns>
	 bool SettingMission(MissionContent mission);

	/// <summary>
	/// 预热机器，如果机器需要预热，则需要实现这个函数
	/// </summary>
	/// <returns>预热工作是否成功</returns>
	 bool PrewarmMachine();

	/// <summary>
	/// 正式开始任务，正式地开始采集
	/// </summary>
	/// <returns>开始任务是否成功</returns>
	 bool StartMission();

	/// <summary>
	/// 暂停任务
	/// </summary>
	/// <returns>暂停结果</returns>
	 bool PauseMission();

	/// <summary>
	/// 停止任务
	/// </summary>
	/// <returns>停止任务结果</returns>
	 bool EndMission();

	/// <summary>
	/// 停止任务之后的一个阶段，用于处理结果
	/// </summary>
	/// <returns></returns>
	 bool AnalyseResult();

private:
	QList<FaroController*> list_pFaro;
};

