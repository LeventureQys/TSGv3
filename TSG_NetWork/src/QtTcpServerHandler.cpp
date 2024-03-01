#include "QtTcpServerHandler.h"
#include "QtTcpBaseHandler.h"

#include <QNetworkInterface>
#include <QThread>

namespace {
	QString getHostIpAddress()
	{
		QString strIpAddress;
		QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
		// 获取第一个本主机的IPv4地址
		int nListSize = ipAddressesList.size();
		for (int i = 0; i < nListSize; ++i)
		{
			if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
				ipAddressesList.at(i).toIPv4Address()) {
				if (ipAddressesList.at(i).toString().contains("192")) {
					strIpAddress = ipAddressesList.at(i).toString();
					break;
				}
			}
		}
		// 如果没有找到，则以本地IP地址为IP
		if (strIpAddress.isEmpty())
			strIpAddress = QHostAddress(QHostAddress::LocalHost).toString();
		return strIpAddress;
	}
}

QtTCPServerHandler::QtTCPServerHandler(QObject* parent) : QObject(parent)
, m_pParent(parent)
{
	connect(&timer_heart, &QTimer::timeout, this, &QtTCPServerHandler::HeartBeatCheck);
}

QtTCPServerHandler::~QtTCPServerHandler()
{
	Close();
}

quint16 QtTCPServerHandler::ServerPort()
{
	if (nullptr != this->m_pTcpServer)
	{
		return this->m_pTcpServer->serverPort();
	}

	return 0;
}

bool QtTCPServerHandler::Open(const quint16 port)
{
	this->m_pTcpServer = new QTcpServer(m_pParent);
	if (!this->m_pTcpServer->listen(QHostAddress(::getHostIpAddress()), port))
	{
		qInfo() << QStringLiteral("##TCPServer_监听端口失败：") + QString::number(port);
		return false;
	}
	connect(this->m_pTcpServer, &QTcpServer::newConnection, this, &QtTCPServerHandler::NewConn);
	this->timer_heart.start(5000);
	return true;
}

void QtTCPServerHandler::Close()
{
	if (Q_NULLPTR != this->m_pTcpServer)
	{
		this->m_pTcpServer->close();
		delete m_pTcpServer;
		this->m_pTcpServer = Q_NULLPTR;
	}

	CloseAllConn();
	this->timer_heart.stop();
}

quint16 QtTCPServerHandler::ConnectCount()
{
	return this->m_qhsSocketCln.size();
}

void QtTCPServerHandler::CloseAllConn()
{
	for (auto it = this->m_qhsSocketCln.begin(); it != this->m_qhsSocketCln.end();)
	{
		(*it)->pTcpSocket->close();

		it = this->m_qhsSocketCln.begin();
	}
}

void QtTCPServerHandler::SendMsg(const QByteArray& bytes, QString clnAddr /*= ""*/, quint16 clnPort /*= 0*/)
{
	auto funSend = [&](ClnInfo* pCln) {
		SendTcp(pCln->pTcpSocket, bytes);

		qInfo() << QStringLiteral("给%1发送TCP：%2").arg(pCln->qsAddr).arg(QString::fromLocal8Bit(bytes));
		};
	if (m_qhsSocketCln.size() == 0) {
		return;
	}
	for (auto& item : m_qhsSocketCln)
	{
		if (!clnAddr.isEmpty() && (clnAddr != item->qsAddr)) continue;

		if (clnPort == item->pTcpSocket->peerPort())
		{
			funSend(item);
			break;
		}
		else if (0 == clnPort)
		{
			funSend(item);
		}
	}
}


QString QtTCPServerHandler::getServerIp() {
	if (this->m_pTcpServer != nullptr)
	{
		return this->m_pTcpServer->serverAddress().toString();
	}
	return QString();
}
bool QtTCPServerHandler::isOpen()
{
	if (this->m_pTcpServer == nullptr) {
		return false;
	}

	return this->m_pTcpServer->isListening();

}
quint16 QtTCPServerHandler::getServerPort()
{
	if (this->m_pTcpServer != nullptr) {
		return this->m_pTcpServer->serverPort();
	}
	return 0;
}

void QtTCPServerHandler::RecvClnMsg() {
	QTcpSocket* pTcpSocket = (QTcpSocket*)sender();
	ClnInfo* pClnInfo = m_qhsSocketCln.value((quint16)pTcpSocket, nullptr);
	if (nullptr == pClnInfo) return;

	QString qsPeerAddr = pClnInfo->pTcpSocket->peerAddress().toString();
	quint16 qnPort = pClnInfo->pTcpSocket->peerPort();

	ReadTCPMsg(pTcpSocket, pClnInfo->nRecvedMsgLen, [&](const QByteArray& bytes) {
		qInfo() << QStringLiteral("#TCPServer_收到来自%1的TCP：%2").arg(qsPeerAddr).arg(QString::fromLocal8Bit(bytes));
		//重置一下心跳数量
		this->map_heartCount.insert(qsPeerAddr, 0);
		emit Sig_RecvClnMsg(qsPeerAddr, qnPort, bytes);
		});
}

void QtTCPServerHandler::NewConn()
{
	QTcpServer* pTcpServer = (QTcpServer*)sender();
	quint16 nServerPort = pTcpServer->serverPort();
	QTcpSocket* pClnTcpSocket = pTcpServer->nextPendingConnection();

	// 登记
	ClnInfo* pNewClnInfo = new ClnInfo;
	pNewClnInfo->pTcpSocket = pClnTcpSocket;
	pNewClnInfo->qsAddr = pClnTcpSocket->peerAddress().toString();
	QTimer* timer = new QTimer(this);
	this->map_heartCount.insert(pNewClnInfo->qsAddr, 0);
	this->m_qhsSocketCln.insert((quint16)pClnTcpSocket, pNewClnInfo);

	// 连接信号
	connect(pClnTcpSocket, &QTcpSocket::readyRead, this, &QtTCPServerHandler::RecvClnMsg);
	connect(pClnTcpSocket, &QTcpSocket::disconnected, this, &QtTCPServerHandler::NewDisConn);
	connect(pClnTcpSocket, &QTcpSocket::disconnected, [=] { pClnTcpSocket->deleteLater(); });
	qInfo() << QStringLiteral("TCPServer_新的连接：(%1)").arg(pClnTcpSocket->peerAddress().toString());

	// 发出信号
	emit Sig_NewConn(pClnTcpSocket->peerAddress().toString(), pClnTcpSocket->peerPort(), pTcpServer->serverPort());
}

void QtTCPServerHandler::NewDisConn()
{
	QTcpSocket* pClnTcpSocket = (QTcpSocket*)sender();

	QMutexLocker lock(&m_qmtForqhsSocketCln);

	QString qsIP = pClnTcpSocket->peerAddress().toString();
	QString qsErr = pClnTcpSocket->errorString();
	qInfo() << QStringLiteral("#TCPServer新的断开：(%1, %2)").arg(qsIP).arg(qsErr);

	quint16 port = pClnTcpSocket->peerPort();
	emit Sig_NewDisConn(qsIP, port);
	emit Sig_NewDisConnEx(qsIP, port, qsErr);


	auto it = m_qhsSocketCln.find((quint16)pClnTcpSocket);
	if (it != m_qhsSocketCln.end())
	{
		m_qhsSocketCln.erase(it);
	}
}

QHash<quint16, ClnInfo*>::iterator QtTCPServerHandler::FindClient(const QString& qsAddress) {
	for (auto item = this->m_qhsSocketCln.begin(); item != this->m_qhsSocketCln.end(); item++) {
		if (item.value()->qsAddr == qsAddress) {
			return item;
		}
	}
	return this->m_qhsSocketCln.end();

}

void QtTCPServerHandler::HeartBeatCheck() {
	for (auto item = map_heartCount.begin(); item != map_heartCount.end(); item++) {
		//如果超过两次心跳没有清空，则断开连接
		if (item.value() > 2) {
			QHash<quint16, ClnInfo*>::iterator it_ClnInfo = FindClient(item.key());
			if (it_ClnInfo == this->m_qhsSocketCln.end()) {
				continue;
			}
			ClnInfo* pClnInfo = it_ClnInfo.value();
			this->m_qhsSocketCln.remove(it_ClnInfo.key());
			if (pClnInfo != nullptr) {
				pClnInfo->pTcpSocket->close();
				qInfo() << QStringLiteral("##TCPServer_心跳检测：(%1)断开连接").arg(item.key());
			}
		}
		this->map_heartCount.insert(item.key(), this->map_heartCount.value(item.key() + 1));
		this->map_heartCount[item.key()]++;
	}
}