#pragma once
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QTime>
#include "qdoublebufferedqueue.h"

enum Grade {
	DEBUG,
	INFO,
	WARNING,
	FAULT
};

extern const QString GRADE_STRING[4];

class SingleLogger : public QThread
{
	SingleLogger(const SingleLogger &) = delete;
	SingleLogger& operator=(const SingleLogger &) = delete;

	SingleLogger(Grade grade = DEBUG) : mGrade(grade), mLogFile(""), mIsRunning(true), mBufferQueue() { }
public:
	static SingleLogger& instance() {
		static SingleLogger logger;
		return logger;
	}

	void setGrade(Grade grade) {
		mGrade = grade;
	}

	Grade grade() const {
		return mGrade;
	}

	void setLogFile(const QString &file) {
		if (0 == file.compare(mLogFile)) return;

		stop();
		mLogFile = file;
		start();
	}

	QString logFile() const {
		return mLogFile;
	}

	void writeLog(Grade grade, const QString &rec) {
		if (grade < mGrade) return;
		
		QString log = tr("%1[%2]%3")
			.arg(QTime::currentTime().toString("hh:mm:ss "))
			.arg(GRADE_STRING[grade]).arg(rec);
		
		mBufferQueue.put(log);
	}

	bool isRunning() const {
		return mIsRunning;
	}

	void stop() {
		mIsRunning = false;
	}

	void start() {
		mIsRunning = true;
		QThread::start();
	}

protected:
	void run() override {
		if (mLogFile.isEmpty())
			mLogFile = "logs/record.log";
		
		QFile file(mLogFile);
		if (!file.open(QIODevice::Append | QIODevice::Text))	{
			mIsRunning = false;
			return;
		}
			
		QTextStream out(&file);
		while (mIsRunning) {			
			QString *rec;
			int count = mBufferQueue.get(&rec);

			if (count > 0) {
				for (int i = 0; i < count; ++i) {
					out << rec[i] << endl;
				}
			}
		}
	}

private:
	virtual ~SingleLogger() {
		mIsRunning = true;
	}

private:
	Grade mGrade;
	bool mIsRunning;
	QString mLogFile;	
	QDoubleBufferedQueue<QString> mBufferQueue;
	
};

#define TRACE_DEBUG(LOG_CONTENT) SingleLogger::instance().writeLog(DEBUG, LOG_CONTENT);
#define TRACE_INFO(LOG_CONTENT) SingleLogger::instance().writeLog(INFO, LOG_CONTENT);
#define TRACE_WARNING(LOG_CONTENT) SingleLogger::instance().writeLog(WARNING, LOG_CONTENT);
#define TRACE_ERROR(LOG_CONTENT) SingleLogger::instance().writeLog(FAULT, LOG_CONTENT);