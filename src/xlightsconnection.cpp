#include "xlightsconnection.h"

#include <QCoreApplication.h>
#include <QElapsedTimer.h>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "spdlog/spdlog.h"

constexpr std::string_view BASEURL = "http://localhost:49913/";

xLightsConnection::xLightsConnection(QObject *parent) :
    QObject(parent)
{
}

bool xLightsConnection::Connect()
{
    GetData(RequestType::GetModels);
    GetData(RequestType::GetSequence);
    return m_connected;
}

void xLightsConnection::AddEffect(QString const& effect, QString const& model, 
        int layer, int startMS, int endMS, QString const& palette, QMap<QString,QString> parmeters)
{
    QString settings;
    for(auto const& key: parmeters.keys())
    {
        settings += QString("%1=%2,").arg(key).arg(parmeters.value(key));
    }
    settings = settings.left(settings.length()-1);

    QMap<QString,QString> all;
    all.insert("target", model);
    all.insert("effect", effect);
    all.insert("layer", QString::number(layer));
    all.insert("startTime", QString::number(startMS));
    all.insert("endTime", QString::number(endMS));
    //all.insert("palette", effect);
    all.insert("settings", settings);
    GetData(RequestType::AddEffect, all);
}

void xLightsConnection::DecodeResponce(QByteArray const& data, RequestType type)
{
	QJsonDocument loadDoc(QJsonDocument::fromJson(data));
	
	switch (type)
	{
	case RequestType::GetSequence:
    {
        QJsonObject xLightObject = loadDoc.object();
		QString seq = xLightObject["seq"].toString();
        QString media = xLightObject["media"].toString();
        int frames = xLightObject["len"].toInt();
        int frameLen = xLightObject["framems"].toInt();
        m_frameTime = frameLen;
        m_frames = frames;
        m_audioFile = media;
        emit UpdateSequence(seq, media, frames, frameLen);

        break;
    }
	case RequestType::GetModels:
    {
        QStringList models;
		QJsonArray modelArray = loadDoc.array();
		for (auto const& model : modelArray)
		{
            models.append(model.toString());
		}
        m_modelList = models;
        emit UpdateModels(models);
        break;
    }
    case RequestType::AddEffect:
    {
        QJsonObject xLightObject = loadDoc.object();
        break;
    }
    default:
        break;
	}
}

QString xLightsConnection::GenerateUrl(RequestType type, QMap<QString,QString> param) const
{
    switch (type)
    {
    case RequestType::GetSequence:
		return QString(BASEURL.data()) + "getOpenSequence";
	case RequestType::GetModels:
		return QString(BASEURL.data()) + "getModels";
    case RequestType::AddEffect:
        QString extra{"?"};
        for(auto const& key: param.keys())
        {
            extra += QString("%1=%2&").arg(key).arg(param.value(key));
        }
        extra = extra.left(extra.length()-1);
		return QString(BASEURL.data()) + "addEffect" + extra;
    }
    return QString();
    //"getOpenSequence"
}

void xLightsConnection::GetData(RequestType type, QMap<QString,QString> param)
{
    auto url = QUrl(GenerateUrl(type, param));
    QNetworkAccessManager manager;
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkReply* response = manager.get(request);

    QElapsedTimer timer;
    timer.start();

    while (!response->isFinished())
    {
        if (timer.elapsed() >= ( 5000))
        {
            response->abort();
            m_connected = false;
            auto logger = spdlog::get("EffectsGen");
            logger->error("Failed to Connect to xLights");
            return;
        }
        QCoreApplication::processEvents();
    }
    m_connected = true;
    auto content = response->readAll();
    response->deleteLater();
    DecodeResponce(content, type);
}