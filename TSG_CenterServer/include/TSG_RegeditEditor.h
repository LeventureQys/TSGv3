#pragma once

#include <QObject>
//注册表控制类，用于专门控制注册表的读写
#include "qsettings.h"
#include "PublicVar.h"
#include <functional>


class TSG_RegeditEditor : public QObject
{
	Q_OBJECT

public:
	TSG_RegeditEditor(QObject* parent = nullptr);
	~TSG_RegeditEditor();
	void InitCheck();	//检查注册表是否存在，如果不存在则需要确认和创建

	void WriteSettings(const QString& key, const QString& value);
	QString GetSettings(const QString& key);
	QList<QString> GetSettingsKeys();




private:
	QSettings* settings = nullptr;
	QMap<QString, QString> map_default;	//默认设置

	/// <summary>
	/// key:设置项 value:设置值
	/// </summary>
	QMap<QString, QString> map_currentSettings;	//当前设置
};
