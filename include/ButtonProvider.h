#ifndef _BUTTONPROVIDER_H_
#define _BUTTONPROVIDER_H_

#include <QObject>
#include <QString>

class ButtonProvider : public QObject
{
Q_OBJECT
public:
	enum ButtonId {
		A,
		B,
		Z
	};
	
	ButtonProvider(QObject *parent = 0);
	virtual ~ButtonProvider();
	
	virtual QString text(const ButtonProvider::ButtonId& id) const = 0;
	virtual bool setPressed(const ButtonProvider::ButtonId& id, bool pressed) const = 0;
	
public slots:
	virtual void refresh() = 0;

signals:
	void buttonTextChanged(const ButtonProvider::ButtonId& id, const QString& text);

private:
	
};

#endif
