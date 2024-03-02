// TSG_Dialog.cpp: 定义应用程序的入口点。
//

#include "TSG_Dialog.h"

using namespace std;

void TSG_Dialog::Init(const QString& str_filePath)
{
	this->str_writting_path = str_filePath;
	this->bln_init = true;
}

bool TSG_Dialog::isInit()
{

	return this->bln_init;
}

void TSG_Dialog::WriteErrorMessage(const QString& ModuleName, const QString& FunctionName, const QString& ErrorMessage)
{
	this->WriteTypeMessage(ModuleName, FunctionName, ErrorMessage, "Error");
}

void TSG_Dialog::WriteNormalMessage(const QString& ModuleName, const QString& FunctionName, const QString& NormalMessage)
{
	this->WriteTypeMessage(ModuleName, FunctionName, NormalMessage, "Normal");
}

void TSG_Dialog::WriteTypeMessage(const QString& ModuleName, const QString& FunctionName, const QString& TypeMessage, const QString& Type)
{
	//需要根据str_writting_path来写入文件
	QDir dir(this->str_writting_path);
	if (dir.exists() == false) {
		if (!dir.mkpath(this->str_writting_path)) {
			this->Sig_Error(ErrorType::CanNotMkpath);
			qDebug() << "#TSG_Dialog -- WriteTypeMessage -- mkpath failed";
			return;
		}
	}
	if (!this->InsertCheck(ModuleName, FunctionName, TypeMessage, Type)) return;
	//找到Type对应的文件夹
	if (!dir.cd(Type)) {
		if (!dir.mkdir(Type)) {
			this->Sig_Error(ErrorType::CanNotMkpath);
			qDebug() << "#TSG_Dialog -- WriteTypeMessage -- mkpath failed";
			return;
		}
		if (!dir.cd(Type)) {
			this->Sig_Error(ErrorType::CanNotCDPath);
			qDebug() << "#TSG_Dialog -- WriteTypeMessage -- CanNotCDPath";
			return;
		}
	}
	QString str_current_datetime = QDate::currentDate().toString("MM-dd");
	QString str_WrittingPath = dir.absolutePath() + QDir::separator() + str_current_datetime + "-" + ModuleName + "-" + FunctionName + ".txt";
	QFile file(str_WrittingPath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
		this->Sig_Error(ErrorType::CanNotOpenFile);
		qDebug() << "#TSG_Dialog -- WriteTypeMessage -- CanNotOpenFile";
		return;
	}
	QTextStream in(&file);
	in << str_current_datetime + "|" + FunctionName + " | " + TypeMessage + "\n";
	file.flush();
	file.close();
}

TSG_Dialog::TSG_Dialog() {
	//这里需要进行异步处理，所以需要注册信号和槽
	connect(this, &TSG_Dialog::Sig_Init, this, &TSG_Dialog::Init);
	connect(this, &TSG_Dialog::Sig_WriteErrorMessage, this, &TSG_Dialog::WriteErrorMessage);
	connect(this, &TSG_Dialog::Sig_WriteNormalMessage, this, &TSG_Dialog::WriteNormalMessage);
	connect(this, &TSG_Dialog::Sig_WriteTypeMessage, this, &TSG_Dialog::WriteTypeMessage);
}

TSG_Dialog::~TSG_Dialog() {

}

bool TSG_Dialog::InsertCheck(const QString& ModuleName, const QString& FunctionName, const QString& TypeMessage, const QString& Type)
{
	if (ModuleName.isEmpty()) {
		this->Sig_Error(ErrorType::ModuleName_Invalid);
		qDebug() << "#TSG_Dialog -- ModuleName_Invalid";
		return false;
	}
	if (FunctionName.isEmpty()) {
		this->Sig_Error(ErrorType::FunctionName_Invalid);
		qDebug() << "#TSG_Dialog -- FunctionName_Invalid";
		return false;
	}
	if (TypeMessage.isEmpty()) {
		this->Sig_Error(ErrorType::Message_Invalid);
		qDebug() << "#TSG_Dialog -- Message_Invalid";
		return false;
	}
	if (Type.isEmpty()) {
		this->Sig_Error(ErrorType::Type_Invalid);
		qDebug() << "#TSG_Dialog -- Type_Invalid";
		return false;
	}
	return true;
}




