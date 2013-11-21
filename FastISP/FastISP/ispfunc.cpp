#include <QMessageBox>
#include "ispfunc.h"

typedef int flag;

enum{
	RICInfo,
	CLRIC,
	WMIC
};

ISPfunc::ISPfunc(QObject *parent)
	: QObject(parent)
{
	QObject::connect(&m_RICINFOWorkThread,SIGNAL(threadoutput(QString,int)),this,SLOT(redictemit(QString,int)));
	QObject::connect(&m_RICINFOWorkThread,SIGNAL(processshow(int)),this,SLOT(redictprocess(int)));
}

ISPfunc::~ISPfunc()
{

}

bool ISPfunc::Enum_Serial_Port( QStringList* PortList )
{
	PortList->clear();//清空列表

	HKEY hKey;  
	LPCTSTR data_Set = _T("HARDWARE\\DEVICEMAP\\SERIALCOMM\\");  
	LONG ret0 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set, 0, KEY_READ, &hKey);  
	if(ret0 != ERROR_SUCCESS)  
		return FALSE;  
	
	DWORD dwIndex = 0;  
	while(1)  {  
		QString a;
		LONG Status;  
		WCHAR Name[256]={0};
		UCHAR szPortName[80]={0};  
		DWORD dwName;  
		DWORD dwSizeofPortName;  
		DWORD Type;  
		dwName = sizeof(Name);  
		dwSizeofPortName = sizeof(szPortName);  
		Status = RegEnumValue(hKey, dwIndex++, Name, &dwName, NULL, &Type,  
			szPortName, &dwSizeofPortName);  

		if((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA))  {  
			for (int i=1;2*i<80;i++)
				szPortName[i] = szPortName[2*i];
			QString qstr((char*)szPortName);
			PortList->push_back(qstr);
		}
		else {  
			break;  
		}  
	}  
	RegCloseKey(hKey); 

	if (PortList->size()>0)
	{
		int size = PortList->size();
		QString* t_portarray = new QString[PortList->size()];
		for (int i = 0;i<PortList->size();i++){
			t_portarray[i] = PortList->at(i);
		}
		QString t_portstr = "";
		for (int i = 0;i+1<PortList->size();i++){
			if (t_portarray[i].right(t_portarray[i].length()-3).toInt()>t_portarray[i+1].right(t_portarray[i+1].length()-3).toInt()){
				t_portstr = t_portarray[i];
				t_portarray[i] = t_portarray[i+1];
				t_portarray[i+1] = t_portstr;
			}
		}
		PortList->clear();
		for (int i = 0;i<size;i++)
			PortList->push_back(t_portarray[i]);
	}

	return(PortList->size() > 0 ? TRUE:FALSE); 
}

void ISPfunc::ReadICInformation( DWORD dwport ,DWORD dwbaudrate)
{
	if (m_RICINFOWorkThread.isRunning())
		m_RICINFOWorkThread.StopThread();

	for (;;)
		if (!m_RICINFOWorkThread.isRunning())
			break;
	
	QString str_port ;
	str_port = str_port.sprintf("%d",dwport);
	
	if (!m_cncom.Open(dwport,dwbaudrate,EVENPARITY,8,ONESTOPBIT))
		return;
	
	emit OutputInfo(QStringLiteral("成功打开串口COM")+str_port+QStringLiteral("!...正在连接...!\n"),0);
	
	m_RICINFOWorkThread.Init(dwport,dwbaudrate,&m_cncom,RICInfo);
	m_RICINFOWorkThread.start();
}

void ISPfunc::redictemit( QString str ,int type)
{
	emit OutputInfo(str,type);
}

bool ISPfunc::GetThreadStatue( void )
{
	return m_RICINFOWorkThread.isRunning();
}

void ISPfunc::ClearIC( DWORD dwport ,DWORD dwbaudrate )
{
	if (m_RICINFOWorkThread.isRunning())
		m_RICINFOWorkThread.StopThread();

	for (;;)
		if (!m_RICINFOWorkThread.isRunning())
			break;

	QString str_port ;
	str_port = str_port.sprintf("%d",dwport);

	if (!m_cncom.Open(dwport,dwbaudrate,EVENPARITY,8,ONESTOPBIT))
		return;

	emit OutputInfo(QStringLiteral("成功打开串口COM")+str_port+QStringLiteral("!...正在连接...!\n"),0);
	
	m_RICINFOWorkThread.Init(dwport,dwbaudrate,&m_cncom,CLRIC);
	m_RICINFOWorkThread.start();
}

void ISPfunc::StopThread( void )
{
	if (m_RICINFOWorkThread.isRunning())
		m_RICINFOWorkThread.StopThread();

	for (;;)
		if (!m_RICINFOWorkThread.isRunning())
			break;
}

void ISPfunc::WMemory( DWORD dwport ,DWORD dwbaudrate ,QStringList liststr )
{
	if (m_RICINFOWorkThread.isRunning())
		m_RICINFOWorkThread.StopThread();

	for (;;)
		if (!m_RICINFOWorkThread.isRunning())
			break;

	QString str_port ;
	str_port = str_port.sprintf("%d",dwport);

	if (!m_cncom.Open(dwport,dwbaudrate,EVENPARITY,8,ONESTOPBIT))
		return;

	emit OutputInfo(QStringLiteral("成功打开串口COM")+str_port+QStringLiteral("!...正在连接...!\n"),0);

	m_RICINFOWorkThread.InitWM(dwport,dwbaudrate,&m_cncom,liststr,WMIC);
	m_RICINFOWorkThread.start();
}

void ISPfunc::redictprocess( int value )
{
	emit ShowProcess(value);
}
