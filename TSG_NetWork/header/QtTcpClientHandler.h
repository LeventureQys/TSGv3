#include <QObject>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>
#include "qtimer.h"


class QtTcpClientHandler : public QObject {
	Q_OBJECT

public:
	QtTcpClientHandler(QObject* obj);
	~QtTcpClientHandler();
	/// <summary>
	/// 启动TCP客户端
	/// </summary>
	/// <param name="qsServerAddr">服务器ip地址</param>
	/// <param name="serverPort">服务器端口号</param>
	void Open(const QString& qsServerAddr, const quint16 serverPort);
	/// <summary>
	/// 关闭服务器
	/// </summary>
	void Close();
	/// <summary>
	/// 向服务端发送消息
	/// </summary>
	/// <param name="bytes">消息内容</param>
	void SendMsg(const QByteArray& bytes);

	/// <summary>
	/// 获得当前ip
	/// </summary>
	/// <returns>ip</returns>
	QString getCurrentIp();

	/// <summary>
	/// 获得当前端口
	/// </summary>
	/// <returns>port</returns>
	quint16 getCurrentPort();
signals:
	/// <summary>
	/// 连接至服务器成功
	/// </summary>
	void Sig_Connected(const QString& qsServerAddr, const quint16 nServerPort);

	/// <summary>
	/// 断开与服务器的连接
	/// </summary>
	/// <param name="qsServerAddr">服务器ip</param>
	/// <param name="nServerPort">端口号</param>
	void Sig_Disconnect(const QString& qsServerAddr, const quint16 nServerPort);

	/// <summary>
	/// 接收到TCP消息
	/// </summary>
	/// <param name="bytes">获得结果</param>
	void Sig_RecvTCP(const QByteArray& bytes);

private slots:
	void ConnectedServer();
	void DisConnectedServer();
	void RecvMsg();
private: 
	qint64 m_nRecvedSize = 0;
	quint16 m_nServerPort = 0;
	QString m_qsServerAddr;
	QTcpSocket* m_pTcpSocket = Q_NULLPTR;
	QTimer timer_heart;
	/// <summary>
	/// 启动心跳包，每五秒发送一次
	/// </summary>
	/// <param name="blnHeartBeat">是否启动</param>
	void StartHeartBeat(bool blnHeartBeat);
	
};