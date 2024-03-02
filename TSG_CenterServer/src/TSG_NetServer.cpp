#include "TSG_NetServer.h"


TSG_NetServer::TSG_NetServer(QObject* parent) : TSG_Framework(parent, this->str_classname)
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
		this->AuthorizeClient(clnAddr, clnPort, bytes);
		emit this->Sig_RecvTCP(bytes);
		});
}

void TSG_NetServer::Close()
{
	if (this->ptr_server == nullptr) return;

	if (this->ptr_server->isOpen()) {
		this->ptr_server->Close();
		delete this->ptr_server;
		this->ptr_server = nullptr;
	}
}

void TSG_NetServer::SendMsg(const QByteArray& bytes)
{
	if (this->ptr_server == nullptr) return;

	if (!this->ptr_server->isOpen()) {
		this->CallErrorMessage("#SendMsg - Server Not Open Yet");
		return;
	}

	this->ptr_server->SendMsg(bytes);
}
void TSG_NetServer::SendMsg_GUI(const QByteArray& bytes)
{
	if (this->ptr_server == nullptr) return;

	if (!this->isOpen()) return;

	for (auto item = this->map_Authorization.begin(); item != this->map_Authorization.end(); ++item) {
		if (item.value().type == ServerType::GUI) {
			this->ptr_server->SendMsg(bytes, item.key());
		}
	}
}
void TSG_NetServer::AuthorizeClient(const QString& clnAddr, const quint16 clnPort, const QByteArray& bytes)
{
	//查不到{则说明不是json字符串，直接退出
	if (!bytes.contains("{")) {
		return;
	}
	QByteArray ret = bytes;
	QString insert = QString::fromUtf8(ret);

	QJsonParseError error;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(insert.toUtf8(), &error);

	if (jsonDoc.isNull()) {
		this->CallErrorMessage("#RecvMessage - QJsonDoc is Null : " + insert);
		return;
	}

	//接到消息，如果是认证信息，则记录下来
	QJsonObject obj = QJsonDocument::fromJson(insert.toUtf8()).object();
	if (obj.contains("InfoType") == false)
		this->CallErrorMessage(QString("#RecvMessage-Server invalid: %1:%2").arg(clnAddr).arg(clnPort));

	QString InfoType = obj.value("InfoType").toString();

	//用于认证的记录
	if (InfoType.contains("Authorize")) {
		//解析认证信息
		QJsonObject obj_content = obj.value("Content").toObject();
		AuthorizeInfo author;
		author.AnalyzeFromString(QJsonDocument(obj_content).toJson());

		//记录下认证信息
		this->map_Authorization.insert(author.ip, author);

		//新设备登录信号
		emit Sig_NewDevice(author.DeviceName, author.ip, author.port);

		//发送服务器的认证信息
		QString authorInfo = this->cur_Author.toString();
		this->SendMsg(authorInfo.toUtf8());

	}
}

QString TSG_NetServer::getIP()
{
	if (this->ptr_server == nullptr) return QString();
	return this->ptr_server->getServerIp();
}