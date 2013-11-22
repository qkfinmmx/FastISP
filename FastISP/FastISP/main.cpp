#include "fastisp.h"
#include <QtWidgets/QApplication>

#include <QtPlugin>
Q_IMPORT_PLUGIN (QWindowsIntegrationPlugin);

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setStyle("fusion");
	FastISP w;
	w.show();
	return a.exec();
}
