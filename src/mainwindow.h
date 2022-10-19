#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "spdlog/spdlog.h"
#include "spdlog/common.h"

#include <memory>
#include <filesystem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }

class QListWidgetItem;
class QListWidget;
class QTableWidget;
class QSettings;
QT_END_NAMESPACE

class xLightsConnection;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public Q_SLOTS:

    void on_actionConnect_xLights_triggered();
    void on_actionClose_triggered();

    void on_actionAbout_triggered();
    void on_actionOpen_Logs_triggered();

    void LogMessage(QString const& message , spdlog::level::level_enum llvl = spdlog::level::level_enum::debug);

private:
    Ui::MainWindow *ui;

    std::shared_ptr<spdlog::logger> logger{ nullptr };
    std::unique_ptr<QSettings> settings{ nullptr };
    std::unique_ptr <xLightsConnection> xLightsConn{ nullptr };
    QString appdir;


};
#endif // MAINWINDOW_H
