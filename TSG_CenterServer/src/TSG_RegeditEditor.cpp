#include "TSG_RegeditEditor.h"
#include "qapplication.h"

TSG_RegeditEditor::TSG_RegeditEditor(QObject* parent)
	: QObject(parent)
{
	settings = new QSettings("South_Software", "TSG @Site");
	this->map_default.insert("DeviceType", DefaultServiceType);
	this->map_default.insert("Language", English);
	this->map_default.insert("IP", DefaultIP);
	this->map_default.insert("Port", DefaultPort);
	this->map_default.insert("DeviceName", Trolley);
	//当前文件的目录
	this->map_default.insert("ProgramFile", QApplication::applicationFilePath());
}

TSG_RegeditEditor::~TSG_RegeditEditor()
{

}

void TSG_RegeditEditor::InitCheck()
{
	this->settings->setValue("ProgramFile", QApplication::applicationFilePath());
	//检查注册表是否存在，如果不存在则需要确认和创建
	for (auto item : this->map_default.keys()) {
		if (!this->settings->contains(item)) {
			//检测到一个不存在，则需要提供一个界面来让用户输入
			this->settings->setValue(item, this->map_default[item]);
		}
	}

	QList<QString> keys = this->map_default.keys();
	this->map_currentSettings.clear();
	for (auto item : keys)
	{
		this->map_currentSettings.insert(item, this->settings->value(item).toString());
	}
}

void TSG_RegeditEditor::WriteSettings(const QString& key, const QString& value)
{
	this->settings->setValue(key, value);
}

QString TSG_RegeditEditor::GetSettings(const QString& key)
{
	return this->settings->value(key).toString();
}

QList<QString> TSG_RegeditEditor::GetSettingsKeys()
{
	return this->settings->allKeys();
}
