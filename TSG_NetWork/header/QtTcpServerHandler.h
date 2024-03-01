/*!
 * \file QtTCPServerHandler.h
 *
 * \author Leventure
 * \date 2024.2
 *
 * 用于支持QT下的TCP服务端应用，使用此类前需要在项目属性中添加network模块支持
 */

#pragma once
#include <QObject>
#include <QHash>
#include <QMutex>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include "qpointer.h"
#include "qtimer.h"
struct ClnInfo {
	qint64 nRecvedMsgLen = 0;
	QString qsAddr;	// 地址有时获取不到，因此开一个值来记录
	QTcpSocket* pTcpSocket = Q_NULLPTR;
};

class QtTCPServerHandler :public QObject {
	Q_OBJECT
public:
	QtTCPServerHandler(QObject* parent);
	~QtTCPServerHandler();
	quint16 ServerPort();
	bool Open(const quint16 port);
	void Close();
	quint16 ConnectCount();
	void CloseAllConn();
	/// <summary>
	/// 发送消息
	/// </summary>
	/// <param name="bytes">消息内容</param>
	/// <param name="clnAddr">目标地址，如果传空，则对所有连接的客户端发送</param>
	/// <param name="clnPort">目标使用的端口，如果传0，则向所有符合地址值的客户端进行发送</param>
	void SendMsg(const QByteArray& bytes, QString clnAddr = "", quint16 clnPort = 0);
	QString getServerIp();
	quint16 getServerPort();

	bool isOpen();
signals:
	void Sig_NewConn(const QString& clnAddr, const quint16 clnPort, const quint16 port);
	void Sig_NewDisConn(const QString& clnAddr, const quint16 clnPort);
	void Sig_NewDisConnEx(const QString& clnAddr, const quint16 clnPort, QString errString);
	void Sig_RecvClnMsg(const QString& clnAddr, const quint16 clnPort, const QByteArray& bytes);
private slots:
	void NewConn();
	void NewDisConn();
	void RecvClnMsg();
	void HeartBeatCheck();
	QHash<quint16, ClnInfo*>::iterator FindClient(const QString& qsAddress);
private:
	QObject* m_pParent = Q_NULLPTR;
	QTcpServer* m_pTcpServer = Q_NULLPTR;	// 根据端口值映射的服务端socket信息
	QHash<quint16, ClnInfo*> m_qhsSocketCln;		// 从QTcpSocket实例地址映射到的socket信息
	QMutex m_qmtForqhsSocketCln;
	QTimer timer_heart;
	QMap<QString, quint8> map_heartCount;
};