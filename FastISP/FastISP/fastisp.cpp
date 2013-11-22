#include <QMessageBox>
#include <QTextStream>
#include "fastisp.h"

FastISP::FastISP(QWidget *parent)
	: QMainWindow(parent)
{
	m_showflag = 0;

	ui.setupUi(this);
	uiinit();	//初始化ui对象

	m_filedialog = new QFileDialog(this,Qt::Dialog);
	m_filedialog->setFileMode(QFileDialog::FileMode::AnyFile);
	m_filedialog->setNameFilter(QStringLiteral("IntelHex(*.hex)"));
	m_filedialog->setViewMode(QFileDialog::ViewMode::Detail);
	
	// 连接信号
	QObject::connect(ui.pushButton_readicinf,SIGNAL(clicked()),this,SLOT(ReadinfbtnClicked()));
	QObject::connect(ui.pushButton_clearic,SIGNAL(clicked()),this,SLOT(ClearbtnClicked()));
	QObject::connect(ui.pushButton_refreshcom,SIGNAL(clicked()),this,SLOT(SearchbtnClicked()));
	QObject::connect(ui.pushButton_start,SIGNAL(clicked()),this,SLOT(ICWriteMemory()));
	QObject::connect(ui.toolButton_path,SIGNAL(clicked()),this,SLOT(ToolbtnClicked()));
	QObject::connect(&m_ispfunc,SIGNAL(OutputInfo(QString,int)),this,SLOT(ShowOutputText(QString,int)));
	QObject::connect(&m_ispfunc,SIGNAL(ShowProcess(int)),this,SLOT(Processvalue(int)));
}

FastISP::~FastISP()
{

}

void FastISP::uiinit( void )
{
	ui.textEdit_path->setHtml(QStringLiteral("<p style=\"color:#C6C6C6;font-size:14px;font-family:\"微软雅黑\";\">Input hex file...</p>"));
	ui.comboBox_comportname->clear();
	if (m_ispfunc.Enum_Serial_Port(&m_comlist)){
		for (int i = 0;i<m_comlist.size();i++){
			ui.comboBox_comportname->addItem(m_comlist.at(i));
		}
	}
	else {
		QMessageBox::warning(this,QStringLiteral("警告 ！"),QStringLiteral("未发现任何串口 ！"));
	}

	ui.comboBox_baudrate->clear();
	QStringList t_combobaudrateList;
	t_combobaudrateList.push_back(QStringLiteral("115200"));
	t_combobaudrateList.push_back(QStringLiteral("57600"));
	t_combobaudrateList.push_back(QStringLiteral("19200"));
	t_combobaudrateList.push_back(QStringLiteral("9600"));
	t_combobaudrateList.push_back(QStringLiteral("4800"));
	ui.comboBox_baudrate->addItems(t_combobaudrateList);

	ui.progressBar->setValue(0);
}

void FastISP::StartbtnClicked( void )
{
	m_showflag = 0;
}

void FastISP::ClearbtnClicked( void )
{
	if (m_ispfunc.GetThreadStatue()){
		m_ispfunc.StopThread();
		ui.plainTextEdit_output->appendPlainText(QStringLiteral("用户中断线程 !"));
		return;
	}

	m_showflag = 0;	
	ui.plainTextEdit_output->clear();

	DWORD t_port,t_baudrate;
	t_port = ui.comboBox_comportname->currentText().right(ui.comboBox_comportname->currentText().length()-3).toInt();
	t_baudrate = ui.comboBox_baudrate->currentText().toInt();
	m_ispfunc.ClearIC(t_port,t_baudrate);
}

void FastISP::ReadflashbtnClicked( void )
{
	m_showflag = 0;
}

void FastISP::ReadinfbtnClicked( void )
{
	if (m_ispfunc.GetThreadStatue()){
		m_ispfunc.StopThread();	
		ui.plainTextEdit_output->appendPlainText(QStringLiteral("用户中断线程 !"));
		return;
	}
	
	m_showflag = 0;
	ui.plainTextEdit_output->clear();

	DWORD t_port,t_baudrate;
	t_port = ui.comboBox_comportname->currentText().right(ui.comboBox_comportname->currentText().length()-3).toInt();
	t_baudrate = ui.comboBox_baudrate->currentText().toInt();
	m_ispfunc.ReadICInformation(t_port,t_baudrate);
}

void FastISP::ShowOutputText( QString str ,int type)
{
	switch(type)
	{
	case 0:
		{
			m_showflag = 0;
			ui.plainTextEdit_output->appendPlainText(str);
			break;
		}
	case 1:
		{
			if (m_showflag == 0){
				m_strsave = ui.plainTextEdit_output->toPlainText();
				m_showflag = 1;
			}
			ui.plainTextEdit_output->setPlainText(m_strsave+str);
			break;
		}
	default:
		break;
	}	
}

void FastISP::SearchbtnClicked( void )
{
	uiinit();
	ui.comboBox_comportname->showPopup();
}

void FastISP::ICWriteMemory( void )
{
	if (m_ispfunc.GetThreadStatue()){
		m_ispfunc.StopThread();	
		ui.progressBar->setValue(0);
		ui.plainTextEdit_output->appendPlainText(QStringLiteral("用户中断线程 !"));
		return;
	}

	ui.progressBar->setValue(0);

	m_showflag = 0;
	ui.plainTextEdit_output->clear();

	QFile t_filehandle(ui.textEdit_path->toPlainText());
	QTextStream t_textstream(&t_filehandle);
	QStringList t_strList;
	if (t_filehandle.open(QFile::ReadOnly | QFile::Truncate)){
		QString t_strValue;
		do {
			t_strValue = t_textstream.readLine();
			t_strList.push_back(t_strValue);
		} while (!t_strValue.isNull());
		t_filehandle.close();
	}
	else{
		QMessageBox::warning(this,QStringLiteral("错误 !"),QStringLiteral("文件打开错误 !"));
		return;
	}

	if (t_strList.size()<2){
		QMessageBox::warning(this,QStringLiteral("错误 !"),QStringLiteral("无法装载文件 !"));
		return;
	}

	QString t_hexfileend = QStringLiteral(":00000001FF");
	if (QString::compare(t_hexfileend,t_strList.at(t_strList.size()-2)) != 0){
		QMessageBox::warning(this,QStringLiteral("错误 !"),QStringLiteral("文件装载失败 !"));
		return;
	}

	DWORD t_port,t_baudrate;
	t_port = ui.comboBox_comportname->currentText().right(ui.comboBox_comportname->currentText().length()-3).toInt();
	t_baudrate = ui.comboBox_baudrate->currentText().toInt();
	m_ispfunc.WMemory(t_port,t_baudrate,t_strList);
}

void FastISP::ToolbtnClicked( void )
{
	m_Filename = "";
	if (m_filedialog->exec()){
		 m_Filename = m_filedialog->selectedFiles().at(0);
	}

	if (!m_Filename.isEmpty()){
		ui.textEdit_path->setHtml(m_Filename);
	}
}

void FastISP::Processvalue( int value )
{
	ui.progressBar->setValue(value);
}
