#include "FaroControl.h"
FaroControl::FaroControl(QObject* parent, const QString& frameName) :TSG_Device<FaroParam, MissionContent>(parent, frameName)
{

}
FaroControl::~FaroControl()
{
}

bool FaroControl::Init(FaroParam param)
{
	for (auto item : param.list_faros) {
		FaroThread* faro_thread = nullptr;
		faro_thread = new FaroThread();
		faro_thread->faro = new FaroController(this);
		faro_thread->thread = new QThread(this);

		faro_thread->faro->moveToThread(faro_thread->thread);
		faro_thread->thread->start();
		connect(faro_thread->faro, &FaroController::Sig_UpdateStatus, this, &FaroControl::slot_UpdateStatus);

		this->map_Listening.insert(item.str_serialNumber, faro_thread);
	}
	return true;
}

void  FaroControl::slot_UpdateStatus(const ScannerParam& param, FaroStatus status) {
	//来自设备的更新状态

	if (status == FaroStatus::INITSDK) {
		if (!this->map_Listening.contains(param.str_serialNumber)) {
			//不存在设备则直接跳出
			return;
		}

		if (this->map_Faro.contains(param.str_serialNumber)) {
			//如果已经是Init的了，那么就不需要再次初始化了
			return;
		}

		this->map_Faro.insert(param.str_serialNumber, this->map_Listening.value(param.str_serialNumber));
	}



}