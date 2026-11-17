#ifndef INPUTPROVIDERWIDGET_H
#define INPUTPROVIDERWIDGET_H

#include <QWidget>
#include <QString>

class InputProviderWidget : public QWidget
{
Q_OBJECT
Q_PROPERTY(QString input READ input)
public:
	enum Result
	{
		Rejected = 0,
		Accepted = 1
	};
	Q_ENUM(Result)

	explicit InputProviderWidget(QWidget *parent = nullptr);

	virtual void setInput(const QString &input) = 0;
	virtual QString input() const = 0;

	Result result() const;
	void resetResult();

public slots:
	void accept();
	void reject();

signals:
	void finished(InputProviderWidget::Result result);

private:
	void finish(Result result);

	Result m_result;
};

#endif // INPUTPROVIDERWIDGET_H
