#include "qobject.h"
#include "qapplication.h"

class Singleton : public QObject {
	 	Q_OBJECT

public:
	static Singleton& ins()
	{
		static Singleton instance(nullptr);  // ����ʵ��
		return instance;
	}


private:
	Singleton(QObject* obj = nullptr);
	const QString str_localPath = QApplication::applicationDirPath();	//��ǰ��ִ���ļ���·���ļ���

	//����IP
	QString str_ip = "127.0.0.1";
	quint16 short_port = 8000;
	QString str_deviceName = "TrolleyAuto";



};