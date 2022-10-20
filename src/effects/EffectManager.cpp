#include "EffectManager.h"

#include "BaseEffect.h"
#include "OnEffect.h"

EffectManager::EffectManager(QObject *parent) :
    QObject(parent)
{
    m_effects.emplace_back(std::make_unique<OnEffect>());
}

QStringList EffectManager::getEffectList() const
{
    QStringList effectNames;
    std::transform(m_effects.cbegin(), m_effects.cend(), std::back_inserter(effectNames),
                   [](auto const& c) { return c->GetName(); });
    return effectNames;
}

BaseEffect* EffectManager::getEffect(QString const& name) const
{
    auto result = std::find_if(begin(m_effects), end(m_effects), [&name](auto const& c)
        {return c->GetName() == name;
        });
    if (result != std::end(m_effects))
    {
        return (* result).get();
    }
    return nullptr;
}