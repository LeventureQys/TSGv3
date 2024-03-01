#include "qobject.h"
#include "PublicVar.h"
class TSG_Framework : public QObject {
	Q_OBJECT
public:
	TSG_Framework(QObject* parent, const QString& frameName) { this->str_framework = frameName; }
	~TSG_Framework() {}

	void CallNormalMessage(const QString& NormalMessage) {
		emit this->Sig_CallMethod(TSG_Dialog, Normal, NormalMessage, this->str_framework);
	}
	void CallErrorMessage(const QString& ErrorMessage) {
		emit this->Sig_CallMethod(TSG_Dialog, Error, ErrorMessage, this->str_framework);
	}
	void CallTypeMessage(const QString& TypeMessage, const QString& type) {
		emit this->Sig_CallMethod(TSG_Dialog, type, TypeMessage, this->str_framework);
	}
signals:
	/// <summary>
	/// 模块对外发送消息
	/// </summary>
	void Sig_CallMethod(const QString& sModule, const QString& sDescribe, const QString& sVariable, const QVariant& extra);

	/// <summary>
	/// 模块接收消息
	/// </summary>
	/// <param name="sModule"></param>
	/// <param name="sDescribe"></param>
	/// <param name="sVariable"></param>
	/// <param name="extra"></param>
	/// <returns></returns>
	virtual qint32 slot_GetCMD(const QString& sModule, const QString& sDescribe, const QString& sVariable, const QVariant& extra) = 0;
private:
	QString str_framework;

};