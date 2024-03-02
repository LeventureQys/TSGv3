// TSG_Dialog.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once
#include "qstring.h"
#include "qfile.h"
#include "qdebug.h"
#include "qdir.h"
#include "qobject.h"
#include <QTextStream>
#include <QFile>
#include "qdatetime.h"
#include "qobject.h"
#include "TSG_Framework.h"



// TODO: 在此处引用程序需要的其他标头。
enum class ErrorType {
	CanNotMkpath,
	CanNotCDPath,
	CanNotOpenFile,
	ModuleName_Invalid,
	FunctionName_Invalid,
	Message_Invalid,
	Type_Invalid,
};

class TSG_Dialog :public QObject {
	Q_OBJECT
public:

	static TSG_Dialog& ins()
	{
		static TSG_Dialog instance;  // 单例实例
		return instance;
	}

	/// <summary>
	/// 检查初始化状态
	/// </summary>
	/// <returns>是否初始化</returns>
	bool isInit();
private slots:
	/// <summary>
	/// 初始化路径
	/// </summary>
	/// <param name="str_filePath">文件路径</param>
	void Init(const QString& str_filePath);


	/// <summary>
	/// 写入错误信息
	/// </summary>
	/// <param name="ModuleName">模块名称</param>
	/// <param name="FunctionName">函数名称</param>
	/// <param name="ErrorMessage">错误内容</param>
	void WriteErrorMessage(const QString& ModuleName, const QString& FunctionName, const QString& ErrorMessage);

	/// <summary>
	/// 写入普通信息
	/// </summary>
	/// <param name="ModuleName">模块名称</param>
	/// <param name="FunctionName">函数名称</param>
	/// <param name="NormalMessage">普通消息内容</param>
	void WriteNormalMessage(const QString& ModuleName, const QString& FunctionName, const QString& NormalMessage);

	/// <summary>
	/// 写入自定义类型信息
	/// </summary>
	/// <param name="ModuleName">模块名称</param>
	/// <param name="FunctionName">函数名称</param>
	/// <param name="TypeMessage">自定义消息内容</param>
	/// <param name="Type">自定义消息类型</param>
	void WriteTypeMessage(const QString& ModuleName, const QString& FunctionName, const QString& TypeMessage, const QString& Type);
private:
	TSG_Dialog();
	~TSG_Dialog();

	QString str_writting_path;
	bool bln_init;

	bool InsertCheck(const QString& ModuleName, const QString& FunctionName, const QString& TypeMessage, const QString& Type);
signals:
	void Sig_Error(ErrorType error);
	void Sig_Init(const QString& str_filePath);
	void Sig_WriteErrorMessage(const QString& ModuleName, const QString& FunctionName, const QString& ErrorMessage);
	void Sig_WriteNormalMessage(const QString& ModuleName, const QString& FunctionName, const QString& NormalMessage);
	void Sig_WriteTypeMessage(const QString& ModuleName, const QString& FunctionName, const QString& TypeMessage, const QString& Type);
};

