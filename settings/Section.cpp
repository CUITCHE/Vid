#include "Section.h"
#include <vector>
#include "Configure.h"

using namespace std;

struct SectionPrivate {
    vector<pair<QString, QString>> kv;
    const Configure *cf;
    const QString name;

    SectionPrivate(const QString &name, const Configure *cf)
        : cf(cf), name(name)
    {}
};

Section::Section(const QString &name, const Configure *cf)
    : d(new SectionPrivate(name, cf))
{

}

Section::~Section() { 
    delete d;
}

QVariant Section::get(const QString &key, const QVariant &defaultValue) const
{
    auto keys = d->cf->childKeysForSection(d->name);
    if (keys.contains(key)) {
        return d->cf->value(d->name, key);
    }
    return defaultValue;
}

