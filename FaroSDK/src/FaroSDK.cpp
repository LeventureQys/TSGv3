#include "FaroSDK.h"

FaroController::FaroController(QObject* obj) : TSG_Framework(obj, str_classname)
{

}

FaroController::~FaroController()
{

}

bool FaroController::InitConnection(const FaroInitParam& param)
{
	InitParam = param;
	return InitHandler(param);
}

bool FaroController::InitHandler(const FaroInitParam& param)
{

	return false;
}