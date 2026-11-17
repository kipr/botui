#include "GuiSettingsWidget.h"
#include "ui_GuiSettingsWidget.h"

#include "ArrayHelpers.h"
#include "MechanicalStyle.h"
#include "Device.h"
#include "RootController.h"
#include "ScreenInversion.h"
#include "ScreenOrientation.h"

#include <QApplication>
#include <QShowEvent>

const static QColor guiColors[] = {
    QColor(50, 50, 50),    // Black
    QColor(255, 50, 50),   // Red
    QColor(255, 192, 203), // Pink
    QColor(50, 50, 255),   // Blue
    QColor(50, 255, 50),   // Green
    QColor(255, 165, 50),  // Orange
    QColor(255, 255, 255)  // White
};

// #define GUI_COLOR_KEY "gui_color"
GuiSettingsWidget::GuiSettingsWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::GuiSettingsWidget())
{
    ui->setupUi(this);
    performStandardSetup(tr("GUI Settings"));

    // ui->colors->setVisible(false);
    // ui->label->setVisible(false);

    /*QColor currentColor = settings->value(GUI_COLOR_KEY, guiColors[0]).value<QColor>();
    quint16 current = 0;
    for(quint16 i = 0; i < sizeof_array(guiColors); ++i) {
        if(currentColor == guiColors[i]) {
            current = i;
            break;
        }
    }
    ui->colors->setCurrentIndex(current);*/

}

GuiSettingsWidget::~GuiSettingsWidget()
{
    delete ui;
}
/*
void GuiSettingsWidget::updateStyle(Device *device)
{
    SettingsProvider *settings = device->settingsProvider();
    QColor currentColor = settings->value(GUI_COLOR_KEY, guiColors[0]).value<QColor>();
    MechanicalStyle *style = qobject_cast<MechanicalStyle *>(QApplication::style());
    if(!style) return;
    style->setUserColor(currentColor);
    updateWidgets();
}

void GuiSettingsWidget::colorChanged(int index)
{
    SettingsProvider *settings = device()->settingsProvider();
    if(!settings) return;

    const QColor &selection = guiColors[index];
    settings->setValue(GUI_COLOR_KEY, selection);
    updateStyle(device());
    settings->sync();
}*/

void GuiSettingsWidget::on_invert_screen_clicked()
{
	ScreenInversion::showPage();
}

void GuiSettingsWidget::showEvent(QShowEvent *event)
{
	StandardWidget::showEvent(event);
	ScreenOrientationStore store;
	bool inverted = false;
	QString error;
	if (store.load(&inverted, &error))
		ui->invert_screen->setText(inverted ? tr("Restore Screen") : tr("Invert Screen"));
}

void GuiSettingsWidget::updateWidgets()
{
    foreach (QWidget *w, QApplication::allWidgets())
    {
        w->update();
    }
}
