#include "LanguageWidget.h"
#include "ui_LanguageWidget.h"

#include "LocaleModel.h"

#include <QDebug>
#include <QSettings>
#include <unistd.h>

LanguageWidget::LanguageWidget(Device *device, QWidget *const parent)
    : StandardWidget(device, parent), ui(new Ui::LanguageWidget) {
    ui->setupUi(this);
    performStandardSetup(tr("Language Widget"));

    LocaleModel *localeModel = new LocaleModel(this);
    ui->languages->setModel(localeModel);

    // Set current index to current locale
    const QString currLocale = QSettings().value("locale", "en").toString();
    for (int i = 0; i < ui->languages->count(); ++i) {
        if (localeModel->locale(localeModel->index(i, 0)).bcp47Name() !=
            currLocale)
            continue;
        ui->languages->setCurrentIndex(i);
        break;
    }
    connect(ui->languages,
            SIGNAL(currentIndexChanged(int)),
            SLOT(currentIndexChanged(int)));
}

LanguageWidget::~LanguageWidget() { delete ui; }

void LanguageWidget::currentIndexChanged(int index) {
    // Create QLocale for selected locale
    if (index < 0) return;
    LocaleModel *const model =
        qobject_cast<LocaleModel *>(ui->languages->model());
    const QLocale locale =
        model->locale(ui->languages->model()->index(index, 0));

    // Update global locale setting
    QSettings settings;
    settings.setValue("locale", locale.bcp47Name());
    qDebug() << "Set locale to " << locale.bcp47Name();
    settings.sync();
    sync();
}