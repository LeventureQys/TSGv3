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
/// ��ʼ���豸�����е��豸������Ҫ�ֶ�����һ����ʼ������������һ����豸����Ҫ�����������
/// </summary>
/// <param name="param">��ʼ������</param>
/// <returns>��ʼ���Ƿ�ɹ�</returns>
	 bool Init(FaroParam param);

	/// <summary>
	/// �����������
	/// </summary>
	/// <param name="param">�������</param>
	/// <returns>���������Ƿ�ɹ�</returns>
	 bool SettingMission(MissionContent mission);

	/// <summary>
	/// Ԥ�Ȼ��������������ҪԤ�ȣ�����Ҫʵ���������
	/// </summary>
	/// <returns>Ԥ�ȹ����Ƿ�ɹ�</returns>
	 bool PrewarmMachine();

	/// <summary>
	/// ��ʽ��ʼ������ʽ�ؿ�ʼ�ɼ�
	/// </summary>
	/// <returns>��ʼ�����Ƿ�ɹ�</returns>
	 bool StartMission();

	/// <summary>
	/// ��ͣ����
	/// </summary>
	/// <returns>��ͣ���</returns>
	 bool PauseMission();

	/// <summary>
	/// ֹͣ����
	/// </summary>
	/// <returns>ֹͣ������</returns>
	 bool EndMission();

	/// <summary>
	/// ֹͣ����֮���һ���׶Σ����ڴ�����
	/// </summary>
	/// <returns></returns>
	 bool AnalyseResult();

private:
	QList<FaroController*> list_pFaro;
};

