#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <QObject>
#include "Section.h"

struct ConfigurePrivate;

class Configure : public QObject
{
    Q_OBJECT
public:
    explicit Configure(QObject *parent = nullptr);
    ~Configure();

    Section section(const QString &sectionName) const;

    static const Configure *shared();

protected:
    QStringList childKeysForSection(const QString &name) const;
    QVariant value(const QString &section, const QString &key) const;
private:
    ConfigurePrivate *d;
    friend class Section;
};

#endif // CONFIGURE_H
