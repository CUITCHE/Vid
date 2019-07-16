#ifndef SECTION_H
#define SECTION_H

#include <QString>
#include <QVariant>

class Configure;

class Section
{
public:
    Section(const QString &name, const Configure *cf);
    ~Section();

    QVariant get(const QString &key, const QVariant &defaultValue = QVariant()) const;
private:
    struct SectionPrivate *d;
};

#endif // SECTION_H
