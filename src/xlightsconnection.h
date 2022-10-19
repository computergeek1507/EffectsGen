#ifndef XLIGHTSCONNECTION_H
#define XLIGHTSCONNECTION_H

#include <QObject>
#include <QTcpSocket>

#include <memory>

class xLightsConnection : QObject
{
    Q_OBJECT
public:
    explicit xLightsConnection(QObject* parent = 0);

    void Connect();


public Q_SLOTS:

    QString GetData(QString const& requestData);

Q_SIGNALS:
    void dataRecieved(QByteArray data);

private:
    bool m_connected{false};

    QString m_audioFile;
    QStringList m_modelList;

    void DecodeResponce(QString const& data);
    void GetModels();
    void GetSequence();
};

#endif 