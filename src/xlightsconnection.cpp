#include "xlightsconnection.h"

#include <QCoreApplication.h>
#include <QElapsedTimer.h>
#include <QNetworkReply>


xLightsConnection::xLightsConnection(QObject *parent) :
    QObject(parent)
{
}

void xLightsConnection::Connect()
{
   
}

void xLightsConnection::GetModels() 
{

}

void xLightsConnection::GetSequence()
{
    "getOpenSequence";
}

void xLightsConnection::DecodeResponce(QString const& data)
{

}

QString xLightsConnection::GetData(QString const& requestData)
{
    QNetworkAccessManager manager;
    QNetworkRequest request(requestData);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkReply* response = manager.get(request);

    QElapsedTimer timer;
    timer.start();

    while (!response->isFinished())
    {
        if (timer.elapsed() >= (5 * 1000))
        {
            response->abort();
            return QString();
        }
        QCoreApplication::processEvents();
    }

    auto content = response->readAll();
    response->deleteLater();
    return QString(content);
}