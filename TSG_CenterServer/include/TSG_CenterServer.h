// TSG_Dialog.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once
#include <QObject>

/// <summary>
/// 中心服务器
/// </summary>
class TSG_CenterServer : public QObject
{
	Q_OBJECT
public:
	TSG_CenterServer(QObject *parent = nullptr);
	~TSG_CenterServer();


};
