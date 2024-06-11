// #ifndef _CREATE3EXAMPLEWIDGET_H_
// #define _CREATE3EXAMPLEWIDGET_H_

// #include "StandardWidget.h"

// #include <QRegularExpression>
// #include <QStringListModel>
// namespace Ui
// {
// 	class Create3ExampleWidget;
// }

// class Create3SensorModel;

// class Create3ExampleWidget : public StandardWidget
// {
// 	Q_OBJECT
// public:
// 	Create3ExampleWidget(Device *device, QWidget *parent = 0);
// 	~Create3ExampleWidget();

// private slots:

// 	int create3Connect();
// 	int isConnected();
// 	void indexChanged(int index);
// 	void run();

// private:
// 	Ui::Create3ExampleWidget *ui;

// 	Create3SensorModel *_model;
// 	double m_setpointVal;
// 	double m_feedbackVal;
// 	int m_position_1;
// 	double m_vel_1;
// 	void update();
// 	QStringListModel *model; 
// 	void updatePids();
// };

// #endif
