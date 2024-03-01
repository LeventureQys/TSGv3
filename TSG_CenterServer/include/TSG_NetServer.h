#include "QtTcpServerHandler.h"

// Path: TSG_Network.h
#include "PublicVar.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "TSG_Framework.h"

class TSG_NetServer : public TSG_Framework {
	Q_OBJECT
public:
	TSG_NetServer(QObject* parent);
	~TSG_NetServer();
	void Open(const QString& qsAddr, const quint16 Port);
	void Close();
	/// <summary>
	/// 向所有客户端发送消息
	/// </summary>
	/// <param name="bytes">消息内容</param>
	void SendMsg(const QByteArray& bytes);
	/// <summary>
	/// 发送消息给GUI程序
	/// </summary>
	/// <param name="bytes">消息内容</param>
	void SendMsg_GUI(const QByteArray& bytes);
	/// <summary>
	/// 发送消息给其他子设备
	/// </summary>
	/// <param name="bytes">消息内容</param>
	void SendMsg_Other(const QByteArray& bytes);

	/// <summary>
	/// 返回GUI程序接受命令的结果
	/// </summary>
	/// <param name="blnSuccess"></param>
	void AcceptCMD_GUI(bool blnSuccess);	//接收命令结果
	bool isOpen();

	QString getIP();
Q_SIGNALS:
	void Sig_Connected(const QString& qsServerAddr);
	void Sig_Disconnect(const QString& qsServerAddr);
	void Sig_RecvTCP(const QByteArray& bytes);
	void Sig_NewDevice(const QString& DeviceName, const QString& ip, const quint16 port);
private:
	const QString str_classname = "TSG_NetServer";
	QtTCPServerHandler* ptr_server;
	bool m_blnIsOpen;
	/// <summary>
	/// key :ip value : AuthorizeInfo
	/// </summary>
	QMap<QString, AuthorizeInfo> map_Authorization;
	/// <summary>
	/// 当前认证信息
	/// </summary>
	AuthorizeInfo cur_Author;
	void AuthorizeClient(const QString& clnAddr, const quint16 clnPort, const QByteArray& bytes);

};