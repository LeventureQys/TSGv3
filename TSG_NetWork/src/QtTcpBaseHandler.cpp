#include "QtTcpBaseHandler.h"
#include <memory>
#include <QtCore/QHash>
#include <QtCore/QMutex>

#pragma pack(1)

struct MsgHeader
{
	/*
	头部12字节
	[0] = 4;
	[1] = 'H';
	[2] = 'E';
	[3] = 'A';
	[4] = 'D';
	[5] = 0;
	[6] = 0;
	[7] = 0;
	[8] = 0;
	[9] = 0;
	[10] = 0;
	[11] = 0;
	*/
	char gc_baseHeader[12] = { 0 };

	/*
	4字节，长度信息：内容长度+1
	*/
	int int_bodyLength = 0;

	/*
	预留字节
	*/
	char gc_reserved[12] = { 0 };
};

// 存储结构
typedef struct tagTCPRecvData
{
	int nExpectSize = 0;	// 期望大小
	int nRecvedLen = 0;		// 已收大小
	QByteArray bytes;

	void Clear()
	{
		nExpectSize = 0;
		nRecvedLen = 0;
		bytes.clear();
	}
}TCPRecvData;

using SPTCPRecvData = std::shared_ptr<TCPRecvData>;

/// <summary>
/// 存放socket与接收数据的映射
/// </summary>
QHash<QTcpSocket*, SPTCPRecvData> hash_socket_spTcpRecvData;

// 互斥锁
QMutex mutex;

void SendTcp(QTcpSocket* socket, const QByteArray& bytes)
{
	/* 消息结构：头 + 内容 + 结尾符0 */
	MsgHeader header;
	header.int_bodyLength = bytes.length() + 1;

	QByteArray bytesHeaderInfo((char*)&header, sizeof(MsgHeader));
	socket->write(bytesHeaderInfo);
	char* pData = new char[header.int_bodyLength];
	memset(pData, 0, header.int_bodyLength);
	memcpy_s(pData, header.int_bodyLength, bytes.data(), bytes.length());
	for (int i = 0; i < header.int_bodyLength; i += 1024)
	{
		socket->write(pData + i, qMin(1024, header.int_bodyLength - i));
	}
	socket->flush();
	delete[] pData;
}

void ReadTCPMsg(QTcpSocket* pTcpSocket, qint64& nCountRecvedMsg, fun_Notice funNotice)
{
	QMutexLocker auto_mutex(&mutex);

	if (!hash_socket_spTcpRecvData.contains(pTcpSocket))
	{
		SPTCPRecvData spTcpRecvData = std::make_shared<TCPRecvData>();
		hash_socket_spTcpRecvData.insert(pTcpSocket, spTcpRecvData);
	}
	SPTCPRecvData spTcpRecvData = hash_socket_spTcpRecvData[pTcpSocket];

	while (pTcpSocket->bytesAvailable() > 0)
	{
		if (0 == spTcpRecvData->nRecvedLen)
		{
			if (pTcpSocket->bytesAvailable() < sizeof(MsgHeader)) return;

			QByteArray bytesHeader = pTcpSocket->read(sizeof(MsgHeader));

			MsgHeader* pHeader = (MsgHeader*)(bytesHeader.data());
			spTcpRecvData->nExpectSize = pHeader->int_bodyLength;
		}

		int nBytesAvailable = pTcpSocket->bytesAvailable();
		if (nBytesAvailable > 0)
		{
			int nThisRecv = qMin(nBytesAvailable, spTcpRecvData->nExpectSize - spTcpRecvData->nRecvedLen);
			spTcpRecvData->bytes += pTcpSocket->read(nThisRecv);
			spTcpRecvData->nRecvedLen += nThisRecv;

			// 其实就是等于，加大于号为了保险...
			if (spTcpRecvData->nRecvedLen >= spTcpRecvData->nExpectSize)
			{
				// 通知
				if (nullptr != funNotice) funNotice(spTcpRecvData->bytes);

				// 重置
				spTcpRecvData->Clear();
			}
		}
	}

	nCountRecvedMsg = spTcpRecvData->nRecvedLen;
}