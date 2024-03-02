#pragma once
#include "qbytearray.h"
#include "qobject.h"
/// <summary>
/// SerialPortParams
/// </summary>
namespace SPP {
	struct SerialMessage {
		bool blnValid = false;
		QByteArray head;
		QByteArray tail;
		QByteArray type;		//消息头
		QByteArray contain;		//消息体
		QByteArray crc;			//CRC校验体
		qint8 length;			//消息体长度
	};



	struct MileageData {
		bool positive;
		quint64 pulseCount;
		quint64 time;
	};

	struct BatteryInfo {
		quint8 status = 0;
		qint16 voltage = 0;
		qint16 current = 0;
		qint16 remainingCapacity = 0;
		qint16 fullCapacity = 0;
		qint16 cycleCount = 0;
	};

	struct InclinometerInfo {
		qint32 X = 0;
		qint32 Y = 0;
		qint64 time = 0;
	};

	struct ImuData {
		QString XRotation = 0;
		QString YRotation = 0;
		QString ZRotation = 0;
		QString XAcceleration = 0;
		QString YAcceleration = 0;
		QString ZAcceleration = 0;
		qint32 status = 0;
	};

	struct ImuRawDataInfo {
		ImuData imu;
		quint64 time = 0;
		quint64 id = 0;
	};

	const QByteArray c_head = QByteArray::fromHex("415464");
	const QByteArray c_tail = QByteArray::fromHex("0D0A");
	const QByteArray imu_head = QByteArray::fromHex("FE81FF55");
	const QByteArray imu_tail = QByteArray::fromHex("0D0A");
	const QByteArray type_test = QByteArray::fromHex("00");		//测试命令

	//Clover
	const QByteArray type_resetTime = QByteArray::fromHex("04");				//时间同步命令
	const QByteArray type_StartGrapping = QByteArray::fromHex("32");			//开启或停止采集命令
	const QByteArray type_CameraEnable = QByteArray::fromHex("33");				//相机使能命令
	const QByteArray type_Light = QByteArray::fromHex("34");					//灯光控制命令
	const QByteArray type_getLightAndFansState = QByteArray::fromHex("36");		//获得灯光及风扇状态
	const QByteArray type_setLightExposureTime = QByteArray::fromHex("40");		//设置灯光曝光时间，指的是灯光的持续时间
	const QByteArray type_SetCloverSpeed = QByteArray::fromHex("42");			//设置电机转速
	const QByteArray type_setIsCloverSpin = QByteArray::fromHex("43");			//是否启动电机旋转
	const QByteArray type_getCloverStatus = QByteArray::fromHex("44");			//获取电机状态
	const QByteArray type_setCloverPosition = QByteArray::fromHex("45");		//设置电机当前方位至-
	const QByteArray type_setCloverGrappingPosition = QByteArray::fromHex("46");//设置码盘的拍照位
	const QByteArray type_getCloverGrappingPosition = QByteArray::fromHex("47");//获取码盘的拍照位
	const QByteArray type_getCameraTimeUpdate = QByteArray::fromHex("F5");		//相机时间数据上传
	const QByteArray type_getScannerTimeUpdate = QByteArray::fromHex("08");		//扫描仪时间数据上传
	//TrolleyAuto
	const QByteArray type_getID = QByteArray::fromHex("01");//获取ID
	//扫描的指令都与法如扫描仪的CAN口通讯有关，但实际上我们不使用CAN口，只是使用串口通讯，所以这里的指令只是为了与法如扫描仪的CAN口通讯而定义的
	const QByteArray type_EndScan = QByteArray::fromHex("02");					//结束扫描
	const QByteArray type_InitScan = QByteArray::fromHex("03");					//初始化扫描
	const QByteArray type_StartScan = QByteArray::fromHex("05");				//开始/暂停扫描

	const QByteArray type_GetTime = QByteArray::fromHex("08");					//获取板载时间
	const QByteArray type_StartTrolley = QByteArray::fromHex("0B");				//开始小车	
	const QByteArray type_StopTrolley = QByteArray::fromHex("0C");				//停止小车
	const QByteArray type_SwitchDirection = QByteArray::fromHex("0D");			//切换方向
	const QByteArray type_SetSpeed = QByteArray::fromHex("0E");					//设置速度
	const QByteArray type_ClearMileage = QByteArray::fromHex("0F");				//清除里程
	const QByteArray type_getTrolleyStatus = QByteArray::fromHex("10");			//获取小车状态
	const QByteArray type_setScannerPower = QByteArray::fromHex("11");			//扫描仪电源控制
	const QByteArray type_getTrolleyMileage = QByteArray::fromHex("13");		//获取小车里程
	const QByteArray type_getBatteryState = QByteArray::fromHex("14");			//获取电池信息
	const QByteArray type_switchEncoder = QByteArray::fromHex("15");			//设置使用哪个编码器
	const QByteArray type_setPushButtonMode = QByteArray::fromHex("16");		//设置按键启停模式
	const QByteArray type_doubleEncoderMileageUpdate = QByteArray::fromHex("F8");//双里程计里程数据上传
	const QByteArray type_singleEncoderMileageUpdate = QByteArray::fromHex("FE");//单里程计里程数据上传

	const QByteArray type_ScannerCANRecv = QByteArray::fromHex("FD");			 //扫描仪CAN口接收数据	
	const QByteArray type_PushButtonMessage = QByteArray::fromHex("FC");		 //面板上的按键信息
	const QByteArray type_InclinometerMessage = QByteArray::fromHex("EF");		 //倾角仪信息上传
	const QByteArray type_ResetCloverPosition = QByteArray::fromHex("45");		//重设码盘中心位置
	const QByteArray type_HardwareButtonInfomation = QByteArray::fromHex("fc");	//物理按键信息上传
	//key: 功能码，value:帧长度

	const QByteArray type_ZS_Mileage = QByteArray::fromHex("FE");
	const QByteArray type_ZS_getCameraTimeUpdate = QByteArray::fromHex("FE");
	const QByteArray type_ZS_IMU = QByteArray::fromHex("FF");
	const QByteArray type_ZS_getStatus = QByteArray::fromHex("10");
	const QByteArray type_ZS_LedLight = QByteArray::fromHex("34");
	const QByteArray type_ZS_CameraEnable = QByteArray::fromHex("33");
	const QByteArray type_ZS_StartGrapping = QByteArray::fromHex("32");
}
