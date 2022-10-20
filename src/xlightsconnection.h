#ifndef XLIGHTSCONNECTION_H
#define XLIGHTSCONNECTION_H

#include <QMap>
#include <QObject>
#include <QString>

#include <memory>

enum class RequestType
{
	GetModels,
	GetSequence,
    AddEffect
};

class xLightsConnection : public QObject
{
    Q_OBJECT
public:
    explicit xLightsConnection(QObject* parent = 0);

    [[nodiscard]] bool Connect();
    [[nodiscard]] int GetFrameTime() const { return m_frameTime;};

    void AddEffect(QString const& effect, QString const& model, 
        int layer, int startMS, int endMS, QString const& palette, QMap<QString,QString> parmeters);

public Q_SLOTS:

    void GetData( RequestType type, QMap<QString,QString> param = QMap<QString,QString>());

Q_SIGNALS:
    void UpdateSequence(QString SequenceName, QString media, int frames, int frameSizeMS);
    void UpdateModels(QStringList models);

private:
    bool m_connected{false};

    QString m_audioFile;
    QStringList m_modelList;

    int m_frameTime{25};
    int m_frames{1000};

    [[nodiscard]] QString GenerateUrl(RequestType type, QMap<QString, QString> param = QMap<QString,QString>()) const;

    void DecodeResponce(QByteArray const& data, RequestType type);
};

#endif