/*!
 * \file QtTcpClientHandler.h
 *
 * \author Leventure
 * \date 2024.2
 *
 * 用于支持QT下的TCP客户端端应用，使用此类前需要在项目属性中添加network模块支持
 */
#include "QtTcpClientHandler.h"
#include "QtTcpBaseHandler.h"
QtTcpClientHandler::QtTcpClientHandler(QObject* parent) : QObject(parent)
, m_pTcpSocket(new QTcpSocket(this)) {
	m_pTcpSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 1024 * 1024 * 100);
	connect(m_pTcpSocket, &QTcpSocket::connected, this, &QtTcpClientHandler::ConnectedServer);
	connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &QtTcpClientHandler::DisConnectedServer);
	connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &QtTcpClientHandler::RecvMsg, Qt::ConnectionType::QueuedConnection);
	connect(&timer_heart, &QTimer::timeout, this, [=]() {
		this->SendMsg(QByteArray("heart"));
		});
}

QtTcpClientHandler::~QtTcpClientHandler() {

	Close();

	if (Q_NULLPTR != m_pTcpSocket)
	{
		delete m_pTcpSocket;
		m_pTcpSocket = Q_NULLPTR;
	}
}

void QtTcpClientHandler::Open(const QString& qsServerAddr, const quint16 serverPort) {
	this->m_qsServerAddr = qsServerAddr;
	this->m_nServerPort = serverPort;
	this->m_pTcpSocket->connectToHost(QHostAddress(m_qsServerAddr), m_nServerPort);
	this->StartHeartBeat(true);
}

void QtTcpClientHandler::Close() {
	this->StartHeartBeat(false);
	this->m_pTcpSocket->close();
}

void QtTcpClientHandler::SendMsg(const QByteArray& bytes) {
	SendTcp(m_pTcpSocket, bytes);
	qInfo() << QStringLiteral("##TcpClient_发送TCP：") + QString::fromLocal8Bit(bytes);
}

QString QtTcpClientHandler::getCurrentIp()
{
	if (this->m_pTcpSocket != Q_NULLPTR) {
		return this->m_pTcpSocket->localAddress().toString();
	}
	return QString();
}

quint16 QtTcpClientHandler::getCurrentPort() {
	if (this->m_pTcpSocket != Q_NULLPTR) {
		return this->m_pTcpSocket->localPort();
	}
	return 0;
}

void QtTcpClientHandler::ConnectedServer()
{
	qInfo() << QStringLiteral("##TcpClient_连接到服务器(%1, %2)").arg(m_qsServerAddr).arg(m_nServerPort);
	emit Sig_Connected(m_qsServerAddr, m_nServerPort);
}

void QtTcpClientHandler::DisConnectedServer() {
	emit Sig_Disconnect(m_qsServerAddr, m_nServerPort);
}

void QtTcpClientHandler::RecvMsg()
{
	ReadTCPMsg(m_pTcpSocket, m_nRecvedSize, [&](const QByteArray& bytes) {
		qInfo() << QStringLiteral("##TcpClient_收到TCP：") + QString::fromLocal8Bit(bytes);
		emit Sig_RecvTCP(bytes);
		});
}

void QtTcpClientHandler::StartHeartBeat(bool blnHeartBeat) {
	if (blnHeartBeat) {
		//3s发送一次心跳包
		timer_heart.start(1000 * 3);
	}
	else {
		timer_heart.stop();
	}
}