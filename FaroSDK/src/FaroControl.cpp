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
		FaroController* pFaro = nullptr;
		pFaro = new FaroController(this);
		if (!pFaro->InitConnection(item)) {
			//如果初始化失败，则释放所有资源，同时返回false
			if (this->list_pFaro.size() > 0) {
				for (qint32 i = 0; i < this->list_pFaro.size(); i++)
				{
					FaroController* faro = this->list_pFaro[i];
					delete faro;
					//delete this->list_pFaro[i];
				}
			}
		}
	}



	return false;
}