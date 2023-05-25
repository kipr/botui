#include "ProgramArgsWidget.h"
#include "ui_ProgramArgsWidget.h"

#include "Device.h"
#include "KeyboardDialog.h"
#include "ProgramArguments.h"
#include "RootController.h"
#include "SystemPrefix.h"

#include <QDebug>
#include <QListWidgetItem>

ProgramArgsWidget::ProgramArgsWidget(const QString &name,
                                     Device *device,
                                     QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::ProgramArgsWidget),
      m_name(name),
      m_dirty(false) {
    ui->setupUi(this);
    performStandardSetup(tr("Program Options"));

    connect(ui->edit, SIGNAL(clicked()), SLOT(edit()));
    connect(ui->up, SIGNAL(clicked()), SLOT(up()));
    connect(ui->down, SIGNAL(clicked()), SLOT(down()));
    connect(ui->add, SIGNAL(clicked()), SLOT(add()));
    connect(ui->remove, SIGNAL(clicked()), SLOT(remove()));
    connect(ui->args,
            SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            SLOT(currentItemChanged(QListWidgetItem *)));

    delete ui->args->takeItem(0);

    // FIXME:
    // ui->args->addItems(ProgramArguments::arguments(kiss::Kar::load(SystemPrefix::ref().rootManager()->archivesPath(m_name))));

    currentItemChanged(0);
}

ProgramArgsWidget::~ProgramArgsWidget() {
    // TODO: What happens if the user downloads a program
    // while this widget is displayed? The program would
    // be overwritten.

    if (!m_dirty) return;
    QStringList args;
    for (int i = 0; i < ui->args->count(); ++i) {
        args << ui->args->item(i)->text();
    }

    /* FIXME:
  const QString path = SystemPrefix::ref().rootManager()->archivesPath(m_name);
  kiss::KarPtr archive = kiss::Kar::load(path);
    ProgramArguments::setArguments(archive, args);
    archive->save(path);
    */
    delete ui;
}

void ProgramArgsWidget::edit() {
    QListWidgetItem *item = ui->args->currentItem();
    if (!item) return;
    KeyboardDialog dialog(tr("Edit Argument"));
    dialog.setInput(item->text());
    RootController::ref().presentDialog(&dialog);
    const QString input = dialog.input();

    if (input.isEmpty()) {
        remove();
        m_dirty = true;
    }
    else if (input != item->text()) {
        item->setText(input);
        m_dirty = true;
    }
}

void ProgramArgsWidget::up() {
    QListWidgetItem *item = ui->args->currentItem();
    if (!item) return;
    const int row = ui->args->row(item);
    QListWidgetItem *above = ui->args->takeItem(row - 1);
    if (!above) return;
    m_dirty = true;
    ui->args->insertItem(row, above);
}

void ProgramArgsWidget::down() {
    QListWidgetItem *item = ui->args->currentItem();
    if (!item) return;
    const int row = ui->args->row(item);
    QListWidgetItem *below = ui->args->takeItem(row + 1);
    if (!below) return;
    m_dirty = true;
    ui->args->insertItem(row, below);
}

void ProgramArgsWidget::add() {
    QListWidgetItem *item = ui->args->currentItem();

    KeyboardDialog dialog(tr("Add Argument"));
    RootController::ref().presentDialog(&dialog);
    const QString input = dialog.input();
    if (input.isEmpty()) return;

    m_dirty = true;
    if (!item)
        ui->args->addItem(input);
    else
        ui->args->insertItem(ui->args->row(item) + 1, input);
}

void ProgramArgsWidget::remove() {
    QListWidgetItem *item = ui->args->currentItem();
    if (!item) return;

    m_dirty = true;
    delete item;
}

void ProgramArgsWidget::currentItemChanged(QListWidgetItem *current) {
    ui->remove->setEnabled(current);
    ui->up->setEnabled(current);
    ui->down->setEnabled(current);
    ui->edit->setEnabled(current);
}
