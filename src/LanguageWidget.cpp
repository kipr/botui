#include "LanguageWidget.h"
#include "ui_LanguageWidget.h"

#include "LocaleModel.h"

#include <QSettings>

LanguageWidget::LanguageWidget(Device *device, QWidget *const parent)
  : StandardWidget(device, parent)
  , ui(new Ui::LanguageWidget)
{
  ui->setupUi(this);
  performStandardSetup(tr("Language Widget"));
  
  LocaleModel *localeModel = new LocaleModel(this);
  ui->languages->setModel(localeModel);
  
  const QString currLocale = QSettings().value("locale", "en").toString();
  for(int i = 0; i < ui->languages->count(); ++i) {
    if(localeModel->locale(localeModel->index(i, 0)).bcp47Name() != currLocale) continue;
    ui->languages->setCurrentIndex(i);
    break;
  }
  connect(ui->languages, SIGNAL(currentIndexChanged(int)), SLOT(currentIndexChanged(int)));
}

LanguageWidget::~LanguageWidget()
{
  delete ui;
}

void LanguageWidget::currentIndexChanged(int index)
{
  if(index < 0) return;
  LocaleModel *model = qobject_cast<LocaleModel *>(ui->languages->model());
  const QLocale locale = model->locale(ui->languages->model()->index(index, 0));
  QSettings settings;
  settings.setValue("locale", locale.bcp47Name());
  settings.sync();
}