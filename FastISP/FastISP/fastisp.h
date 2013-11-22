#ifndef FASTISP_H
#define FASTISP_H

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include "ui_fastisp.h"
#include "ispfunc.h"

class FastISP : public QMainWindow
{
	Q_OBJECT

public:
	FastISP(QWidget *parent = 0);
	~FastISP();

private:
	Ui::FastISPClass ui;
	ISPfunc m_ispfunc;
	QFileDialog* m_filedialog;

private:
	QStringList m_comlist;
	QString m_strsave;
	QString m_Filename;

private:
	int m_showflag;

private:
	void uiinit(void);

public slots:
	void StartbtnClicked(void);
	void ClearbtnClicked(void);
	void ReadflashbtnClicked(void);
	void ReadinfbtnClicked(void);
	void ShowOutputText(QString str , int type);
	void SearchbtnClicked(void);
	void ICWriteMemory(void);
	void ToolbtnClicked(void);
	void Processvalue(int value);
};

#endif // FASTISP_H
