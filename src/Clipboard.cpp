#include "Clipboard.h"

void Clipboard::setContents(const QVariant &contents,
                            const Clipboard::Type &type) {
    m_contents = contents;
    m_type = type;
}

const QVariant &Clipboard::contents() const { return m_contents; }

const Clipboard::Type &Clipboard::type() const { return m_type; }

void Clipboard::clearContents() { setContents(QVariant(), Clipboard::Unknown); }