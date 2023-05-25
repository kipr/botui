#ifndef _CONCURRENTCOMPILE_H_
#define _CONCURRENTCOMPILE_H_

#include <QFileInfo>
#include <QObject>
#include <QRunnable>
#include <QString>

#include <kar/kar.hpp>
#include <pcompiler/output.hpp>

class Device;

class ConcurrentCompile : public QObject, public QRunnable {
    Q_OBJECT
  public:
    ConcurrentCompile(const QFileInfo &file,
                      const kiss::KarPtr &archive,
                      Device *device);

    virtual void run();

    Q_PROPERTY(void *userData READ userData WRITE setUserData);
    void setUserData(void *data);
    void *userData() const;

    Q_PROPERTY(bool finished READ isFinished)
    bool isFinished() const;
    const Compiler::OutputList &output() const;

  signals:
    // sender() is causing issues across threads,
    // so we're just going to pass ourselves.
    void compileStarted(const QString &name, ConcurrentCompile *self);
    void compileFinished(const Compiler::OutputList &output,
                         ConcurrentCompile *self);

  private:
    QFileInfo m_file;
    kiss::KarPtr m_archive;
    Device *m_device;
    void *m_userData;
    Compiler::OutputList m_output;
};

#endif
