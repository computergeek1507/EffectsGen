#ifndef BASEPROPERTY_H
#define BASEPROPERTY_H

#include <QString>

struct BaseProperty
{
	BaseProperty(){};
	BaseProperty(QString const& name) : Name(name) {}
	virtual ~BaseProperty() = default;

	QString Name;
	virtual QString ToString() const = 0;

	virtual void ChangeValue(QString const& val) = 0;
};

struct IntProperty : BaseProperty
{
	IntProperty(QString const& name, int val) : BaseProperty(name), Value(val) {};
	int Value{100};
	QString ToString() const override { return QString::number(Value); }

	void ChangeValue(QString const& val) override
	{
		Value = val.toInt();
	}
};

struct DoubleProperty : BaseProperty
{
	DoubleProperty(QString const& name,double val) : BaseProperty(name), Value(val) {};
	double Value;
	QString ToString() const override { return QString::number(Value, 10, 1); }

	void ChangeValue(QString const& val) override
	{
		Value = val.toDouble();
	}
};

struct BoolProperty : BaseProperty
{
	BoolProperty(QString const& name,bool val) : BaseProperty(name), Value(val) {};
	bool Value{false};
	QString ToString() const override { return QString::number((int)Value); };

	void ChangeValue(QString const& val) override
	{
		Value = val.toInt();
	}
};

struct StringProperty : BaseProperty
{
	StringProperty(QString const& name,QString val) : BaseProperty(name), Value(val) {};
	QString Value;
	QString ToString() const override { return Value; };

	void ChangeValue(QString const& val) override
	{
		Value = val;
	}
};

#endif