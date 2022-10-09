#include "mainwindow.h"

#include "./ui_mainwindow.h"

#include "config.h"

#include <QMessageBox>
#include <QDesktopServices>
#include <QSettings>
#include <QFileDialog>
#include <QTextStream>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTableWidget>
#include <QThread>
#include <QInputDialog>
#include <QCommandLineParser>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardPaths>

#include "spdlog/spdlog.h"

#include "spdlog/sinks/qt_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include <filesystem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
	QCoreApplication::setApplicationName(PROJECT_NAME);
    QCoreApplication::setApplicationVersion(PROJECT_VER);
    ui->setupUi(this);

	auto const log_name{ "log.txt" };

	appdir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	std::filesystem::create_directory(appdir.toStdString());
	QString logdir = appdir + "/log/";
	std::filesystem::create_directory(logdir.toStdString());

	try
	{
		auto file{ std::string(logdir.toStdString() + log_name) };
		auto rotating = std::make_shared<spdlog::sinks::rotating_file_sink_mt>( file, 1024 * 1024, 5, false);

		logger = std::make_shared<spdlog::logger>("EffectsGen", rotating);
		logger->flush_on(spdlog::level::debug);
		logger->set_level(spdlog::level::debug);
		logger->set_pattern("[%D %H:%M:%S] [%L] %v");
	}
	catch (std::exception& /*ex*/)
	{
		QMessageBox::warning(this, "Logger Failed", "Logger Failed To Start.");
	}

	setWindowTitle(windowTitle() + " v" + PROJECT_VER);

	settings = std::make_unique< QSettings>(appdir + "/settings.ini", QSettings::IniFormat);

	RedrawRecentList();

	bool ssl = QSslSocket::supportsSsl();
	QString const sslFile = QSslSocket::sslLibraryBuildVersionString();

	if (!ssl)
	{
		QString const text = QStringLiteral("OpenSSL not found on your computer.<br>Please Install " ) + sslFile + QStringLiteral("<br><a href='http://slproweb.com/products/Win32OpenSSL.html'>OpenSSL Download</a>");
		QMessageBox::warning(this, "OpenSSL", text);
	}
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_EEPROM_triggered()
{
	QString const EEPROM = QFileDialog::getOpenFileName(this, "Select EEPROM File", settings->value("last_project").toString(), tr("EEPROM Files (*.bin);;All Files (*.*)"));
	if (!EEPROM.isEmpty())
	{
		
	}
}

void MainWindow::on_actionDownload_EEPROM_triggered()
{
	
}

void MainWindow::on_actionClose_triggered()
{
	close();
}

void MainWindow::on_actionAbout_triggered()
{
	QString text = QString("Effects Gen v%1<br>QT v%2<br><br>Icons by:")
		.arg(PROJECT_VER, QT_VERSION_STR) +
		QStringLiteral("<br><a href='http://www.famfamfam.com/lab/icons/silk/'>www.famfamfam.com</a>");
		//http://www.famfamfam.com/lab/icons/silk/
	QMessageBox::about( this, "About Effects Gen", text );
}

void MainWindow::on_actionOpen_Logs_triggered()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(appdir + "/log/"));
}

void MainWindow::on_menuRecent_triggered()
{
	auto recentItem = qobject_cast<QAction*>(sender());
	if (recentItem && !recentItem->data().isNull())
	{
		auto const project = qvariant_cast<QString>(recentItem->data());
		
	}
}

void MainWindow::on_actionClear_triggered()
{
	ui->menuRecent->clear();
	settings->remove("Recent_ProjectsList");

	ui->menuRecent->addSeparator();
	ui->menuRecent->addAction(ui->actionClear);
}
void MainWindow::AddRecentList(QString const& file)
{
	auto recentProjectList = settings->value("Recent_ProjectsList").toStringList();

	recentProjectList.push_front(file);
	recentProjectList.removeDuplicates();
	if (recentProjectList.size() > 10)
	{
		recentProjectList.pop_back();
	}
	settings->setValue("Recent_ProjectsList", recentProjectList);
	settings->sync();
	RedrawRecentList();
}

void MainWindow::RedrawRecentList()
{
	ui->menuRecent->clear();
	auto recentProjectList = settings->value("Recent_ProjectsList").toStringList();
	for (auto const& file : recentProjectList)
	{
		if (!QFile::exists(file))
		{
			continue;
		}
		QFileInfo fileInfo(file);
		auto* recentpn = new QAction(this);
		recentpn->setText(fileInfo.dir().dirName() + "/" + fileInfo.fileName());
		recentpn->setData(fileInfo.absoluteFilePath());
		ui->menuRecent->addAction(recentpn);
		connect(recentpn, &QAction::triggered, this, &MainWindow::on_menuRecent_triggered);
	}

	ui->menuRecent->addSeparator();
	ui->menuRecent->addAction(ui->actionClear);
}

void MainWindow::LogMessage(QString const& message, spdlog::level::level_enum llvl)
{
	logger->log(llvl, message.toStdString());
}

