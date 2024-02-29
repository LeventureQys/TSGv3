/*!
 * \file QtTcpBaseHandler.h
 *
 * \author Leventure
 * \date 2024.2.29
 *
 * ����֧��QT�µ�TCP��������ʹ�ô���ǰ��Ҫ����Ŀ���������networkģ��֧��
 * ������һ����ӵ�����Ƽ��򵥵��߳�����
 */

#pragma once
#include <functional>

#include <QtNetwork/QTcpSocket>

void SendTcp(QTcpSocket* pTcpSocket, const QByteArray& bytes);

using fun_Notice = std::function<void(const QByteArray&)>;
void ReadTCPMsg(QTcpSocket* pTcpSocket, qint64& nCountRecvedMsg, fun_Notice funNotice);