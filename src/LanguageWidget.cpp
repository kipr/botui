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
  
  ui->languages->setModel(new LocaleModel(this));
  QSettings settings;
  QLocale current(settings.value("locale", "en").toString());
  for(int i = 0; i < ui->languages->count(); ++i) {
    
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
  const QLocale locale = model->locale(ui->languages->rootModelIndex().child(index, 0));
  QSettings settings;
  settings.setValue("locale", locale.bcp47Name());
  settings.sync();
}