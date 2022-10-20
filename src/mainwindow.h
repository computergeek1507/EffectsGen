#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "xlightsconnection.h"
#include "./effects/EffectManager.h"

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

//class xLightsConnection;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString FormatTime(int ticks) const;

public Q_SLOTS:

    void on_actionConnect_xLights_triggered();
    void on_actionClose_triggered();

    void on_actionAbout_triggered();
    void on_actionOpen_Logs_triggered();

    void on_hs_StartTime_valueChanged(int);
    void on_hs_EndTime_valueChanged(int);

    void on_cb_Effects_currentTextChanged(QString);
    void on_cb_Models_currentTextChanged(QString);
    void on_tw_Properties_cellDoubleClicked(int, int);

    void on_pb_add_clicked();

    void LogMessage(QString const& message, spdlog::level::level_enum llvl = spdlog::level::level_enum::debug);

    void on_ModelsUpdated(QStringList models);
    void on_SequenceUpdated(QString name, QString audio, int frames, int frameSizeMS);

private:
    Ui::MainWindow *m_ui;

    std::shared_ptr<spdlog::logger> m_logger{ nullptr };
    std::unique_ptr<QSettings> m_settings{ nullptr };
    std::unique_ptr<xLightsConnection> m_xLightsConn{ nullptr };
    std::unique_ptr<EffectManager> m_effectManager{ nullptr };
    QString m_appdir;
};
#endif // MAINWINDOW_H
