#ifndef _KOVANBUTTONPROVIDER_H_
#define _KOVANBUTTONPROVIDER_H_

#include "ButtonProvider.h"

class AbstractButton;

namespace Kovan
{
	class ButtonProvider : public ::ButtonProvider
	{
	Q_OBJECT
	public:
		ButtonProvider(QObject *parent = 0);
		~ButtonProvider();
		virtual QString text(const ButtonProvider::ButtonId& id) const;
		virtual bool setPressed(const ButtonProvider::ButtonId& id, bool pressed) const;
	public slots:
		virtual void refresh();
	private:
		AbstractButton *m_a;
		AbstractButton *m_b;
		AbstractButton *m_z;
	};
}

#endif
