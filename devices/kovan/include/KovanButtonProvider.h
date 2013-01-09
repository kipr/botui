#ifndef _KOVANBUTTONPROVIDER_H_
#define _KOVANBUTTONPROVIDER_H_

#include "ButtonProvider.h"

class AbstractTextButton;

namespace Kovan
{

	class ButtonProvider : public ::ButtonProvider
	{
	Q_OBJECT
	public:
		ButtonProvider(QObject *parent = 0);
		~ButtonProvider();
		
		virtual bool isExtraShown() const;
		virtual QString text(const ButtonProvider::ButtonId& id) const;
		virtual bool setPressed(const ButtonProvider::ButtonId& id, bool pressed) const;
		
	public slots:
		virtual void reset();
		virtual void refresh();
	private:
		AbstractTextButton *lookup(const ButtonProvider::ButtonId& id) const;
		
		bool m_shown;
	};
}

#endif
