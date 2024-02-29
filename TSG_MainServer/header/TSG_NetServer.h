#include "QtTcpServerHandler.h"

// Path: TSG_Network.h

class TSG_NetServer : public QObject {
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
Q_SIGNALS:
	void Sig_Connected(const QString& qsServerAddr);
	void Sig_Disconnect(const QString& qsServerAddr);
	void Sig_RecvTCP(const QByteArray& bytes);
	void Sig_RecvNewFile(const QString& filePath);
	void Sig_CallMethod(const QString& sModule, const QString& sDescribe, const QString& sVariable, const QVariant& extra);

	void Sig_NewDevice(const QString& DeviceName, const QString& ip, const quint16 port);
private:

	QtTCPServerHandler* ptr_server;
	bool m_blnIsOpen;

};