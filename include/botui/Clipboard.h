#ifndef _CLIPBOARD_H_
#define _CLIPBOARD_H_

#include "Singleton.h"

#include <QVariant>

class Clipboard : public Singleton<Clipboard> {
  public:
    enum Type { Unknown, File, Folder, Text };

    void setContents(const QVariant &contents, const Type &type);
    const QVariant &contents() const;
    const Type &type() const;
    void clearContents();

  private:
    QVariant m_contents;
    Type m_type;
};

#endif
