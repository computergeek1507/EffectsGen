#ifndef BASEEFFECT_H
#define BASEEFFECT_H

#include <QMap>
#include <QObject>
#include <QString>

#include <memory>

#include "BaseProperty.h"

struct BaseEffect
{
	virtual std::string GetType() const { return typeid(this).name(); }
	virtual QString GetName() const = 0;

	std::vector<std::unique_ptr<BaseProperty>> Properties;
};
#endif