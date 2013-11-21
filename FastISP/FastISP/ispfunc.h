#ifndef ISPFUNC_H
#define ISPFUNC_H

#include <QObject>
#include <QPlainTextEdit>
#include "CnComm.h"
#include "readicinfoworkthread.h"

class ISPfunc : public QObject
{
	Q_OBJECT

public:
	ISPfunc(QObject *parent = 0);
	~ISPfunc();

	enum ReadICINFRtv{
		success = 1,
		openporterror
	};

signals:
	void OutputInfo(QString str, int type);
	void ShowProcess(int value);

public slots:
	void redictemit(QString str,int type);
	void redictprocess(int value);

private:
	ReadICINFOWorkThread m_RICINFOWorkThread;
	CnComm m_cncom;

public:
	bool Enum_Serial_Port(QStringList* PortList);
	void ReadICInformation(DWORD dwport ,DWORD dwbaudrate);
	bool GetThreadStatue(void);
	void ClearIC( DWORD dwport ,DWORD dwbaudrate );
	void StopThread(void);
	void WMemory( DWORD dwport ,DWORD dwbaudrate ,QStringList liststr);
};

#endif // ISPFUNC_H
