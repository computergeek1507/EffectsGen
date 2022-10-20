#ifndef ONEFFECT_H
#define ONEFFECT_H

#include <QMap>
#include <QObject>
#include <QString>

#include <memory>

#include "BaseEffect.h"

struct OnEffect : BaseEffect
{
	OnEffect();
	QString GetName() const override { return "On"; };

};
#endif