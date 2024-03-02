#include "qobject.h"
#include "qdatetime.h"
#include "PublicVar.h"
#include "qdir.h"
#include "TSG_Framework.h"
/// <summary>
/// 任务管理类
/// 任务分为两级，一级是项目，一级是任务
/// 项目是必须要设置的，也就是某个地方的项目
/// 任务是不一定需要设置的，可以自动生成，也可以手动设定，在不进行手动设置的时候都需要进行自动生成
/// 任务的存在是中心化的，所有的任务管理都只存在于中心CentralMainService中，子设备不需要过多关心任务的模式本身，而是只需要知道他们需要做什么
/// 
/// 现在info.json除了用于存储任务本身的信息，还用于存储设备配置，统一在info.json中进行统一调配和控制
/// 不存在更改单个设备的情况，只存在更改整个任务的情况
/// </summary>

class MissionController : public TSG_Framework {
	Q_OBJECT
public:
	MissionController(QObject* parent);
	~MissionController();
	/// <summary>
	/// 初始化
	/// </summary>
	/// <param name="ApplicationPath">应用程序路径</param>
	bool Init(const QString& ApplicationPath);

	/// <summary>
	/// 获得任务列表，调用时会刷新任务列表
	/// </summary>
	/// <returns>任务列表</returns>
	QList<QString> getMissionList();

	/// <summary>
	/// 创建一个默认任务
	/// </summary>
	/// <param name="MissionName"></param>
	/// <returns></returns>
	bool CreateMission(const QString& MissionName);
	bool CreateMission(const QString& MissionName, const MissionContent& Content);

	/// <summary>
	/// 删除一个任务
	/// </summary>
	/// <param name="MissionName"></param>
	/// <returns></returns>
	bool DeleteMission(const QString& MissionName);

	/// <summary>
	/// 设置任务属性
	/// </summary>
	/// <param name="MissionName">任务名称</param>
	/// <param name="valueName">属性名称</param>
	/// <param name="variant">内容</param>
	/// <returns>返回值</returns>
	bool setMissionValue(const QString& MissionName, const MissionContent& Content);

	/// <summary>
	/// 启动指定任务
	/// </summary>
	/// <param name="MissionName">任务名称</param>
	/// <returns>是否正确启动</returns>
	bool StartMission(const QString& MissionName);

	MissionContent getCurrentMissionContent();
signals:

	/// <summary>
	/// 启动任务后发送任务信息
	/// </summary>
	void Sig_StartMissionInfo(const MissionContent& Content);

private:
	QString str_ApplicationPath;
	QString str_CurrentMissionPath;
	QMap<QString, MissionContent> map_MissionContent;

	const QString str_classname = "MissionController";
	QString currentMissionName = "";
};

