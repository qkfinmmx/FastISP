#ifndef READICINFOWORKTHREAD_H
#define READICINFOWORKTHREAD_H

#include <QThread>
#include <QPlainTextEdit>
#include "CnComm.h"

typedef int flag ;

class ReadICINFOWorkThread : public QThread
{
	Q_OBJECT

public:
	ReadICINFOWorkThread(QObject *parent = 0);
	~ReadICINFOWorkThread();

	enum ReadICINFRtv{
		success = 1,
		openporterror
	};

signals:
	void threadoutput(QString str,int type);
	void processshow(int value);

private:
	CnComm* m_cncom;
	flag m_flag;
	int m_type;
	QStringList m_strwrite;

private:
	DWORD m_portnum;
	DWORD m_baudrate;

private:
	QString TransBootloadVersion(unsigned char ucWord);
	QString Transuchtoqstring(unsigned char ucWord);

public:
	void Init(DWORD dwportnum,DWORD dwbaudrate,CnComm* pCncom,int type);
	void InitWM(DWORD dwportnum,DWORD dwbaudrate,CnComm* pCncom,QStringList str,int type);
	void StopThread(void);

private:
	bool converttohexstr(QString str,unsigned char* uchhex);

protected:
	virtual void run();
};

#endif // READICINFOWORKTHREAD_H
