#include "TSG_NetServer.h"

TSG_NetServer::TSG_NetServer(QObject* parent) : QObject(parent)
{

}

void TSG_NetServer::Open(const QString& qsAddr, const quint16 Port) {
	this->ptr_server = new QtTCPServerHandler(this);

	connect(this->ptr_server, &QtTCPServerHandler::Sig_NewConn, this,
		[=](const QString& clnAddr, const quint16 clnPort, const quint16 port) {
			emit this->Sig_Connected(clnAddr);
		});

	connect(this->ptr_server, &QtTCPServerHandler::Sig_NewDisConn, this, [=](const QString& clnAddr, const quint16 clnPort) {
		emit this->Sig_Disconnect(clnAddr);
		});

	connect(this->ptr_server, &QtTCPServerHandler::Sig_RecvClnMsg, this, [=](const QString& clnAddr, const quint16 clnPort, const QByteArray& bytes) {
		emit this->Sig_RecvTCP(bytes);
		});
}