#include "Configure.h"
#include <QSettings>

struct ConfigurePrivate {
    QSettings *s;
    ConfigurePrivate() {
        QSettings::setDefaultFormat(QSettings::IniFormat);
        s = new QSettings();
    }
    ~ConfigurePrivate() {
        delete s;
    }
};

Configure::Configure(QObject *parent)
    : QObject(parent)
    , d(new ConfigurePrivate)
{

}

Configure::~Configure()
{
    delete d;
}

Section Configure::section(const QString &sectionName) const
{
    return Section(sectionName, this);
}

const Configure *Configure::shared()
{
    static auto instance = new Configure();
    return instance;
}

QStringList Configure::childKeysForSection(const QString &name) const
{
    d->s->beginGroup(name);
    return d->s->childKeys();
}

QVariant Configure::value(const QString &section, const QString &key) const
{
    d->s->beginGroup(section);
    return d->s->value(key);
}
