/*!
 * \file QtTcpBaseHandler.h
 *
 * \author Leventure
 * \date 2024.2.29
 *
 * 用于支持QT下的TCP公共处理，使用此类前需要在项目属性中添加network模块支持
 * 包括了一定的拥塞控制及简单的线程锁控
 */

#pragma once
#include <functional>

#include <QtNetwork/QTcpSocket>

void SendTcp(QTcpSocket* pTcpSocket, const QByteArray& bytes);

using fun_Notice = std::function<void(const QByteArray&)>;
void ReadTCPMsg(QTcpSocket* pTcpSocket, qint64& nCountRecvedMsg, fun_Notice funNotice);