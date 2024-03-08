#pragma comment(linker, "\"/manifestdependency:type='win32' name='FARO.LS' version = '1.1.0.0' processorArchitecture = 'x86'publicKeyToken = '1d23f5635ba800ab'\"") //  Edit version!

#import <iQOpen.dll> no_namespace
#import <FARO.LS.SDK.dll> no_namespace

#include "FaroParams.h"
#include "TSG_Framework.h"
#include "qprocess.h"
#include "qdir.h"
#include "qdebug.h"
#include "qthread.h"
class FaroInitParam {
public:
	QString serialNumber;
	QString scanKey;
	QString scanIP;
	QString conType = "LAN";
};

class FaroController : public TSG_Framework
{
public:
	FaroController(QObject* obj);
	~FaroController();

	bool InitConnection(const MissionContent& mission);
	bool setDeviceParam(const ScannerParam& param);

	bool StartMission();
	bool PauseMission();
	bool EndMission();
private:

	const QString str_classname = "FaroController";
	HelicalRecordingStatus currentRecordingState = HelicalRecordingStatus::HRSUnknown;
	MissionContent mission;
	ScannerParam param;
	FaroStatus Status = FaroStatus::DEFAULT;
	IiQLicensedInterfaceIfPtr liPtr = nullptr;
	IScanCtrlSDKPtr scanCtrl = nullptr;
	QString missionPath = "";
	const qint32 int_timeout_limit = 90;	//超时限制
	QString str_pointcloudPath;


	bool TryPingDevice(const ScannerParam& InitParam);
	bool InitHandler(const ScannerParam& mission);
	bool ResetHandler();
	bool ConnectFaro();
	bool InitMission(const MissionContent& mission);
	qint32 checkScanProgress();
	/// <summary>
	/// 旋转镜头的操作，镜头需要启动旋转状态才允许进入到扫描模式中
	/// </summary>
	/// <returns></returns>
	bool StartScanRotation();
	/// <summary>
	/// 开始镜头旋转
	/// </summary>
	/// <returns></returns>
	bool StartRecording();

	/// <summary>
	/// 暂停采集任务
	/// </summary>
	/// <returns></returns>
	bool PauseRecording();

	/// <summary>
	/// 停止采集
	/// </summary>
	/// <returns></returns>
	bool StopRecording();
};