#include "ProgramsWidget.h"
#include "ArchivesModel.h"
#include "AreYouSureDialog.h"
#include "CompileHelpers.h"
#include "CompileProvider.h"
#include "ConcurrentCompile.h"
#include "Config.h"
#include "Device.h"
#include "EditorWidget.h"
#include "KeyboardDialog.h"
#include "LogDialog.h"
#include "MenuBar.h"
#include "Program.h"
#include "ProgramArgsWidget.h"
#include "ProgramArguments.h"
#include "ProgramWidget.h"
#include "RootController.h"
#include "StatusBar.h"
#include "SystemPrefix.h"
#include "ui_ProgramsWidget.h"

#include <QDebug>
#include <QItemDelegate>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMessageBox>
#include <QThreadPool>
#include <QTimer>

class ItemDelegate : public QItemDelegate {
  public:
    ItemDelegate(QObject *parent = 0) : QItemDelegate(parent) {}

    virtual QSize sizeHint(const QStyleOptionViewItem &option,
                           const QModelIndex &index) const {
        return QSize(0, 22);
    }
};

ProgramsWidget::ProgramsWidget(Device *device, QWidget *parent)
    : StandardWidget(device, parent),
      ui(new Ui::ProgramsWidget),
      m_model(new ArchivesModel(device, this)) {
    ui->setupUi(this);
    performStandardSetup(tr("Programs"));

    ui->programs->setItemDelegate(new ItemDelegate(this));
    ui->programs->setModel(m_model);
    connect(ui->run, SIGNAL(clicked()), SLOT(run()));
    connect(ui->edit, SIGNAL(clicked()), SLOT(edit()));
    connect(ui->add, SIGNAL(clicked()), SLOT(add()));
    connect(ui->compile, SIGNAL(clicked()), SLOT(compile()));
    connect(ui->remove, SIGNAL(clicked()), SLOT(remove()));
    connect(ui->args, SIGNAL(clicked()), SLOT(args()));
    connect(ui->transfer, SIGNAL(clicked()), SLOT(transfer()));

    // TODO: remove these once the buttons all work
    ui->edit->setVisible(true);
    ui->add->setVisible(false);
    ui->remove->setVisible(false);
    ui->args->setVisible(false);
    ui->transfer->setVisible(false);
    ui->compile->setVisible(true);

    connect(ui->programs->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            SLOT(update()));

    QTimer *timer = new QTimer(this);
    timer->start(2500);
    connect(timer, SIGNAL(timeout()), SLOT(update()));

    update();
}

ProgramsWidget::~ProgramsWidget() { delete ui; }

void ProgramsWidget::run() {
    if (Program::instance()->isRunning()) return;

    QModelIndexList currents =
        ui->programs->selectionModel()->selectedIndexes();
    if (currents.size() != 1) return;

    const QString name = m_model->name(currents[0]);

    // Make sure binary exists for this project
    const QDir projDir(botui::pathToKISS + name);
    qDebug() << name;
    if (!projDir.exists("bin/botball_user_program")) {
        QMessageBox::warning(
            this,
            tr("No Executable"),
            tr("No executable exists for the selected proejct."));
        return;
    }

    ui->run->setEnabled(false);

    ProgramWidget *programWidget =
        new ProgramWidget(Program::instance(), device());
    const bool success =
        Program::instance()->start(projDir.filePath("bin/botball_user_program"),
                                   projDir.filePath("data/"));

    if (success)
        RootController::ref().presentWidget(programWidget);
    else
        delete programWidget;

    ui->run->setEnabled(true);
}

void ProgramsWidget::edit() {
    QModelIndexList currents =
        ui->programs->selectionModel()->selectedIndexes();
    if (currents.size() != 1) return;

    const QString name = m_model->name(currents[0]);
    qDebug() << "edit clicked for " << name;

    // const QString archivePath =
    // SystemPrefix::ref().rootManager()->archivesPath(name); qDebug() <<
    // "archivePath" << archivePath; kiss::KarPtr archive =
    // kiss::Kar::load(archivePath); if(archive.isNull()) return;

    const QString projectPath = botui::pathToKISS + name;
    qDebug() << "project path: " << projectPath;

    EditorWidget *editor = new EditorWidget(device());
    editor->setProjectPath(projectPath);
    // editor->setArchive(archive);
    // editor->setSavePath(archivePath);

    RootController::ref().presentWidget(editor);
}

void ProgramsWidget::add() {
    KeyboardDialog keyboard(tr("Program Name"));
    if (RootController::ref().presentDialog(&keyboard) != QDialog::Accepted)
        return;
    const QString name = keyboard.input();
    if (name.isEmpty()) return;

    /* FIXME
    const QString archivePath =
    SystemPrefix::ref().rootManager()->archivesPath(name);

    if(!kiss::Kar::load(archivePath).isNull()) {
        AreYouSureDialog dialog;
        dialog.setDescription(tr("You're about to overwrite the program \"%1\".
    Continue?\n").arg(name)); if(RootController::ref().presentDialog(&dialog) !=
    QDialog::Accepted) return;
    }

    kiss::KarPtr archive = kiss::Kar::create();
    if(!archive->save(archivePath)) {
        qWarning() << "Failed to create new archive with the name" << name;
        return;
    }

    */
    EditorWidget *editor = new EditorWidget(device());
    // FIXME: editor->setArchive(archive);
    // FIXME: editor->setSavePath(archivePath);
    RootController::ref().presentWidget(editor);
}

void ProgramsWidget::args() {
    QModelIndexList currents =
        ui->programs->selectionModel()->selectedIndexes();
    if (currents.size() != 1) return;
    const QString name = m_model->name(currents[0]);
    RootController::ref().presentWidget(new ProgramArgsWidget(name, device()));
}

void ProgramsWidget::compile() {
    QModelIndexList currents =
        ui->programs->selectionModel()->selectedIndexes();
    if (currents.size() != 1) return;

    const QString name = m_model->name(currents[0]);

    qDebug() << "compile clicked for " << name;

    QDir kissPath(botui::pathToKISS);
    if (!kissPath.cd(name)) qDebug() << "failed to enter directory";

    // since the command is natively executed, make sure to replace spaces with
    // "\ " so that they will be recognizable
    const QDir projectPath(kissPath);
    const QString backspacedProjectPath =
        projectPath.absolutePath().replace(" ", "\\ ");

    const QString includeDir(backspacedProjectPath + "/include/");
    const QString srcDir(backspacedProjectPath + "/src/*.c*");
    QString binFilePath = backspacedProjectPath + "/bin/botball_user_program";

    QString compileCommand = "g++ -o " + binFilePath +
                             " -lkipr -lpthread -lm -I " + includeDir + " " +
                             srcDir;

    // any compile arguments from config.json
    const QDir dataDir(projectPath.absolutePath() + "/data/");
    if (dataDir.exists(dataDir.absolutePath() + "/config.json")) {
        QFile file(dataDir.absolutePath() + "/config.json");
        qDebug() << "exists!! and opening ";
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "failed to open existing config.json";
            return;
        }
        QByteArray bytes = file.readAll();
        QJsonDocument document(QJsonDocument::fromJson(bytes));
        QJsonObject object(document.object());
        if (object.keys().contains("compilerArgs")) {
            compileCommand += " ";
            QJsonArray array(object["compilerArgs"].toArray());
            for (QJsonValue v : array) {
                compileCommand += v.toString() + " ";
            }
        }

        file.close();
    }
    else {
        qDebug() << "no config.json to open";
    }
    qDebug() << compileCommand;

    QProcess compiler;
    compiler.start("sh", QStringList() << "-c" << compileCommand);
    compiler.waitForFinished();
    QString standardOutput = compiler.readAllStandardOutput();
    QString standardError = compiler.readAllStandardError();
    qDebug() << "standard output is" << compiler.readAllStandardOutput();
    qDebug() << "error is " << compiler.readAllStandardError();
    if (compiler.exitCode() != 0) {
        QMessageBox::warning(this, "failed", standardError);
    }
    else {
        qDebug() << "succeeded";
        QMessageBox::information(this, "succeeded", "Compilation succeeded.");
    }
    qDebug() << "exit code is " << compiler.exitCode();
}

void ProgramsWidget::remove() {
    QModelIndexList currents =
        ui->programs->selectionModel()->selectedIndexes();
    if (currents.size() != 1) return;
    SystemPrefix::ref().rootManager()->uninstall(m_model->name(currents[0]));
    update();
}

void ProgramsWidget::transfer() {
    QModelIndexList currents =
        ui->programs->selectionModel()->selectedIndexes();
    if (currents.size() != 1) return;
    const QString name = m_model->name(currents[0]);
    /* FIXME: const kiss::KarPtr archive =
  kiss::Kar::load(SystemPrefix::ref().rootManager()->archivesPath(name)); const
  QDir flashDrive("/kovan/media/sda1/transfers/" + name);
  QDir().mkpath(flashDrive.path());
  archive->extract(flashDrive.path());
  */
}

void ProgramsWidget::compileStarted(const QString &name,
                                    ConcurrentCompile *compiler) {
    if (!compiler) {
        qWarning() << "sender is null";
        return;
    }

    LogDialog *log = reinterpret_cast<LogDialog *>(compiler->userData());
    log->setStatus(tr("Compiling..."));
}

void ProgramsWidget::compileFinished(const Compiler::OutputList &output,
                                     ConcurrentCompile *compiler) {
    if (!compiler) {
        qWarning() << "sender is null";
        return;
    }

    LogDialog *log = reinterpret_cast<LogDialog *>(compiler->userData());
    foreach (const Compiler::Output &out, output) {
        log->appendText(CompileHelpers::postProcess(out.output()));
        log->appendText(CompileHelpers::postProcess(out.error()));
    }

    // This is more so something will be in the log.
    // It's disconcerting when it's entirely empty.
    log->appendText(tr("Compile finished."));

    log->setStatus(Compiler::Output::isSuccess(output) ? tr("Success!")
                                                       : tr("Failure."));
    log->setDismissable(true);
}

void ProgramsWidget::update() {
    QModelIndexList currents =
        ui->programs->selectionModel()->selectedIndexes();
    const bool good = currents.size() == 1;
    ui->run->setEnabled(good && !Program::instance()->isRunning());
    ui->edit->setEnabled(good);
    ui->remove->setEnabled(good);
    ui->args->setEnabled(good);
    const QDir flashDrive("/kovan/media/sda1");
    ui->transfer->setEnabled(good && flashDrive.exists());
}
