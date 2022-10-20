#ifndef EFFECTMANAGER_H
#define EFFECTMANAGER_H

#include <QMap>
#include <QObject>
#include <QString>

#include <memory>

struct BaseEffect;

class EffectManager : public QObject
{
    Q_OBJECT
public:
    explicit EffectManager(QObject* parent = 0);

    QStringList getEffectList() const;
    BaseEffect* getEffect(QString const& name) const;

public Q_SLOTS:


Q_SIGNALS:


private:
    std::vector<std::unique_ptr<BaseEffect>> m_effects;
};
#endif