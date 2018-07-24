#pragma once


template<class Type>
class ConcurrentQueue
{
	ConcurrentQueue &operator=(const ConcurrentQueue &) = delete;
	ConcurrentQueue(const ConcurrentQueue &) = delete;
public:
	ConcurrentQueue()
		: mQueue()
		, mMutex()
		, mCondition() {}
	virtual ~ConcurrentQueue() {}

	void Push(Type record) {
		QMutexLocker locker(&mMutex);
		mQueue.enqueue(record);
		mCondition.wakeOne();
	}

	bool Pop(Type &record, bool isBlocked = true) {
		QMutexLocker locker(&mMutex);
		if (isBlocked) {
			//QMutexLocker locker(&mMutex);
			while (mQueue.empty()) {
				mCondition.wait(&mMutex);
			}
		}
		else {
			//QMutexLocker locker(&mMutex);
			if (mQueue.empty()) {
				return false;
			}
		}

		record = std::move(mQueue.head());
		mQueue.dequeue();
		return true;
	}

	qint32 Size() {
		QMutexLocker locker(&mMutex);
		return mQueue.size();
	}

	bool Empty() {
		QMutexLocker locker(&mMutex);
		return mQueue.empty();
	}

	void Clear() {
		QMutexLocker locker(&mMutex);
		mQueue.clear();
	}
private:
	QQueue<Type> mQueue;
	mutable QMutex mMutex;
	QWaitCondition mCondition;
};
