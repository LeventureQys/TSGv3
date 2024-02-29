//#include "TSG_NetServer.h"
//#include "qjsondocument.h"
//#include "qjsonobject.h"
//#include "qjsonvalue.h"
//TSG_NetServer::TSG_NetServer(QObject* parent) : QObject(parent) {
//
//}
//TSG_NetServer::~TSG_NetServer() {
// 
//}
//void TSG_NetServer::Open(const QString& qsAddr, const quint16 Port) {
//	this->ptr_serWver = new QtTCPServerHandler(this);
//	connect(this->ptr_server, &QtTCPServerHandler::Sig_NewConn, this,
//		[=](const QString& clnAddr, const quint16 clnPort, const quint16 port) {
//			emit this->Sig_Connected(clnAddr);
//		});
//	connect(this->ptr_server, &QtTCPServerHandler::Sig_NewDisConn, this, [=](const QString& clnAddr, const quint16 clnPort) {
//		emit this->Sig_Disconnect(clnAddr);
//		});
//	connect(this->ptr_server, &QtTCPServerHandler::Sig_RecvClnMsg, this, [=](const QString& clnAddr, const quint16 clnPort, const QByteArray& bytes) {
//		emit this->Sig_RecvTCP(bytes);
//		});
//}
//void TSG_NetServer::CallDialog(const QString& FunctionName, const QString& Message)
//{
//	QJsonObject obj;
//	obj.insert("ModuleName", str_class_name);
//	obj.insert("FunctionName", FunctionName);
//	obj.insert("NormalMessage", Message);
//	emit Sig_CallMethod("TSG_Dialog", "WriteNormalMessage", QJsonDocument(obj).toJson(), QVariant());
//	emit Sig_CallMethod("Message", "CallDialog", QJsonDocument(obj).toJson(), QVariant());
//}
//
//void TSG_NetServer::CallError(const QString& FunctionName, const QString& Message)
//{
//
//	QJsonObject obj;
//	obj.insert("ModuleName", str_class_name);
//	obj.insert("FunctionName", FunctionName);
//	obj.insert("ErrorMessage", Message);
//	emit Sig_CallMethod("TSG_Dialog", "WriteErrorMessage", QJsonDocument(obj).toJson(), QVariant());
//	emit Sig_CallMethod("Error", "CallError", QJsonDocument(obj).toJson(), QVariant());
//
//}
//void TSG_NetServer::ReceiveTCP(const QString& clnAddr, const quint16 clnPort, const QByteArray& bytes) {
//	QByteArray ret = bytes;
//	QString insert = QString::fromUtf8(ret);
//
//	QJsonParseError error;
//	QJsonDocument jsonDoc = QJsonDocument::fromJson(insert.toUtf8(), &error);
//	if (jsonDoc.isNull()) {
//		this->CallError("ReceiveTCP", "##Insert : " + insert.toUtf8());
//		return;
//	}
//
//	//接到消息，如果是认证信息，则记录下来
//	QJsonObject obj = QJsonDocument::fromJson(insert.toUtf8()).object();
//
//	if (obj.contains("InfoType") == false)
//		this->CallError("RecvMessage_Server", QString("RecvMessage_Server invalid: %1:%2").arg(clnAddr).arg(clnPort));
//
//}