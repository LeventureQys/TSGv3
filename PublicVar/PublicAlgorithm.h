#pragma once
///公用算法组件
#include <QObject>
#include "qqueue.h"
#include "qbytearray.h"
#include "qmutex.h"
//这是一个线程安全的队列,只做两件事，读和写，写入操作不需要保证线程安全，但是读操作需要保证线程安全
class ThreadSafeQueue
{
public:
	ThreadSafeQueue() {}
	~ThreadSafeQueue() {}

	void append(const QByteArray& qba) {
		QMutexLocker locker(&mutex);
		vec.enqueue(qba);
	}

	// 获取并清空整个队列
	QQueue<QByteArray> dequeueAll() {
		QMutexLocker locker(&mutex);
		QQueue<QByteArray> result = vec;
		vec.clear();
		return result;
	}

	qint32 size() {
		return this->vec.size();
	}
private:
	QMutex mutex;            // 互斥锁
	QQueue<QByteArray> vec;
};
