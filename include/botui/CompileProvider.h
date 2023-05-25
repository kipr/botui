#ifndef _COMPILEPROVIDER_H_
#define _COMPILEPROVIDER_H_

#include <QObject>
#include <QString>

#include <kar/kar.hpp>
#include <pcompiler/output.hpp>

class CompileProvider : public QObject {
    Q_OBJECT
  public:
    CompileProvider(QObject *parent = 0);
    virtual ~CompileProvider();

    void setBinariesPath(const QString &binariesPath);
    const QString &binariesPath() const;
    virtual Compiler::OutputList compile(const QString &name,
                                         const kiss::KarPtr &program,
                                         const QString &install_dir) = 0;

  signals:
    void compileFinished(const Compiler::OutputList &output);

  private:
    QString m_binariesPath;
};

#endif
