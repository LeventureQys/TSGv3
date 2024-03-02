#include "qobject.h"
#include "qapplication.h"

class Singleton : public QObject {
	 	Q_OBJECT

public:
	static Singleton& ins()
	{
		static Singleton instance(nullptr);  // 单例实例
		return instance;
	}


private:
	Singleton(QObject* obj = nullptr);
	const QString str_localPath = QApplication::applicationDirPath();	//当前可执行文件的路径文件夹

	//本地IP
	QString str_ip = "127.0.0.1";
	quint16 short_port = 8000;
	QString str_deviceName = "TrolleyAuto";



};