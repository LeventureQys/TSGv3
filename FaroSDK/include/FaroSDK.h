#pragma comment(linker, "\"/manifestdependency:type='win32' name='FARO.LS' version = '1.1.0.0' processorArchitecture = 'x86'publicKeyToken = '1d23f5635ba800ab'\"") //  Edit version!

#import <iQOpen.dll> no_namespace
#import <FARO.LS.SDK.dll> no_namespace

#include "FaroParam.h"
#include "TSG_Framework.h"

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

	bool InitConnection(const FaroInitParam& param);

private:
	const QString str_classname = "FaroController";
	HelicalRecordingStatus currentRecordingState = HelicalRecordingStatus::HRSUnknown;
	FaroInitParam InitParam;


	bool InitHandler(const FaroInitParam& param);
	FaroStatus currentStatus = FaroStatus::DEFAULT;


};