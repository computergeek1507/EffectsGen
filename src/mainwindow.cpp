#include "mainwindow.h"

#include "./ui_mainwindow.h"

#include "config.h"

#include <QMessageBox>
#include <QDesktopServices>
#include <QSettings>
#include <QTextStream>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTableWidget>

#include <QInputDialog>

#include <QNetworkAccessManager>
#include <QStandardPaths>

#include "spdlog/spdlog.h"

#include "spdlog/sinks/qt_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include <filesystem>

#include "./effects/BaseEffect.h"
#include "./effects/BaseProperty.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
	QCoreApplication::setApplicationName(PROJECT_NAME);
    QCoreApplication::setApplicationVersion(PROJECT_VER);
    m_ui->setupUi(this);

	auto const log_name{ "log.txt" };

	m_appdir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	std::filesystem::create_directory(m_appdir.toStdString());
	QString logdir = m_appdir + "/log/";
	std::filesystem::create_directory(logdir.toStdString());

	try
	{
		auto file{ std::string(logdir.toStdString() + log_name) };
		auto rotating = std::make_shared<spdlog::sinks::rotating_file_sink_mt>( file, 1024 * 1024, 5, false);

		m_logger = std::make_shared<spdlog::logger>("EffectsGen", rotating);
		m_logger->flush_on(spdlog::level::debug);
		m_logger->set_level(spdlog::level::debug);
		m_logger->set_pattern("[%D %H:%M:%S] [%L] %v");
	}
	catch (std::exception& /*ex*/)
	{
		QMessageBox::warning(this, "Logger Failed", "Logger Failed To Start.");
	}

	setWindowTitle(windowTitle() + " v" + PROJECT_VER);

	m_settings = std::make_unique< QSettings>(m_appdir + "/settings.ini", QSettings::IniFormat);
	m_effectManager = std::make_unique< EffectManager>();
	m_xLightsConn = std::make_unique< xLightsConnection>();
	connect(m_xLightsConn.get(), &xLightsConnection::UpdateModels, this, &MainWindow::on_ModelsUpdated);
	connect(m_xLightsConn.get(), &xLightsConnection::UpdateSequence, this, &MainWindow::on_SequenceUpdated);
	
	m_ui->cb_Effects->addItem(QString());
	m_ui->cb_Effects->addItems(m_effectManager->getEffectList());
	
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
    delete m_ui;
}

void MainWindow::on_actionConnect_xLights_triggered()
{
	auto connected = m_xLightsConn->Connect();
	if(!connected)
	{
		QMessageBox::warning(this, "xLights Error", "Failed to Connect to xLights, Enable API thought the xFade Port in the Preferences");
	}
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
	QDesktopServices::openUrl(QUrl::fromLocalFile(m_appdir + "/log/"));
}

void MainWindow::on_hs_StartTime_valueChanged(int value)
{
	//ms not ticks
	m_ui->le_StartTime->setText( FormatTime(value) );
}

void MainWindow::on_hs_EndTime_valueChanged(int value)
{
	m_ui->le_EndTime->setText( FormatTime(value ) );
}

void MainWindow::on_ModelsUpdated(QStringList models)
{
	m_ui->cb_Models->clear();
	for(auto const& model : models)
	{
		m_ui->cb_Models->addItem(model);
	}
}

void MainWindow::on_SequenceUpdated(QString name, QString audio, int frames, int frameSizeMS)
{
	m_ui->hs_StartTime->setMaximum(frames);
	m_ui->hs_EndTime->setMaximum(frames);
	m_ui->hs_StartTime->setTickInterval(frameSizeMS);
	m_ui->hs_EndTime->setTickInterval(frameSizeMS);
	m_ui->hs_StartTime->setValue(0);
	m_ui->hs_EndTime->setValue(frames);
	//m_ui->sb_StartTime->setSingleStep(frameSizeMS);
	//m_ui->sb_EndTime->setSingleStep(frameSizeMS);
	//m_ui->sb_StartTime->setMaximum(frames * frameSizeMS);
	//m_ui->sb_EndTime->setMaximum(frames * frameSizeMS);
	if(QFile::exists(audio))
	{
		m_ui->widget->setSource(audio);
	}
}

void MainWindow::on_cb_Effects_currentTextChanged(QString name)
{
	m_ui->tw_Properties->clearContents();
	m_ui->tw_Properties->setRowCount(0);

	auto SetReadItem = [&](int row, int col, QString const& text)
	{
		m_ui->tw_Properties->setItem(row, col, new QTableWidgetItem());
		m_ui->tw_Properties->item(row, col)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		m_ui->tw_Properties->item(row, col)->setText(text);
	};

	auto SetItem = [&](int row, int col, QString const& text)
	{
		m_ui->tw_Properties->setItem(row, col, new QTableWidgetItem());
		m_ui->tw_Properties->item(row, col)->setText(text);
	};

	auto eff = m_effectManager->getEffect(name);
	if(nullptr == eff)
	{
		return;
	}

	m_ui->tw_Properties->setRowCount(eff->Properties.size());
	//eff->Properties
	int row{ 0 };
	for(auto const& prop : eff->Properties)
	{
		SetReadItem(row, 0, prop->Name);
		SetItem(row, 1, prop->ToString());
		++row;
	}

	m_ui->tw_Properties->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::on_tw_Properties_cellDoubleClicked(int row, int column)
{
	if(0 == column)
	{
		m_ui->tw_Properties->removeRow(row);
	}
	
	//auto header{ ui->twParts->horizontalHeaderItem(column)->text() };
	//auto value{ ui->twParts->item(row, column)->text() };
	//
	//bool ok;
	//QString text = QInputDialog::getText(this, header,
	//	header, QLineEdit::Normal,
	//	value, &ok);
	//if (ok && !text.isEmpty())
	//{
	//	ui->twParts->item(row, column)->setText(text);
	//	schematic_adder->UpdatePart(ui->twParts->item(row, 0)->text(),
	//								ui->twParts->item(row, 1)->text(),
	//								ui->twParts->item(row, 2)->text(),
	//								ui->twParts->item(row, 3)->text(),
	//								ui->twParts->item(row, 4)->text(), row);
	//}
}

void MainWindow::on_pb_add_clicked()
{
	QMap<QString,QString> properties;
	for(int x = 0; x < m_ui->tw_Properties->rowCount(); x++)
	{
		auto key = m_ui->tw_Properties->item(x, 0)->text();
		auto value = m_ui->tw_Properties->item(x, 1)->text();
		properties.insert(key, value);
	}
	auto model = m_ui->cb_Models->currentText();
	auto effect = m_ui->cb_Effects->currentText();
	auto layer = m_ui->sb_Layer->value();
	auto start = m_ui->hs_StartTime->value();
	auto end = m_ui->hs_EndTime->value();
	auto len = m_ui->sb_Length->value();
	auto gap = m_ui->sb_Gap->value();
	for(int i = start; i<end; i +=(len + gap))
	{
		m_xLightsConn->AddEffect(effect, model, layer,i, i+len, "", properties);
	}
}

QString MainWindow::FormatTime(int ticks) const
{
	return QTime::fromMSecsSinceStartOfDay(ticks ).toString("mm:ss.zzz");
}

void MainWindow::LogMessage(QString const& message, spdlog::level::level_enum llvl)
{
	m_logger->log(llvl, message.toStdString());
}