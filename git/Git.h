#ifndef GIT_H
#define GIT_H

#include <QObject>
#include "model/LoggerI.h"
#include <functional>

struct GitPrivate;

class Git : public QObject, public LoggerI<Git>
{
    Q_OBJECT
public:
    explicit Git(const QString &rootDirPath, QObject *parent = nullptr);

    // 执行git status后的回调函数类型。入参为：新增的文件列表、修改的文件列表、已删除的文件列表
    using GitStatusCallback = std::function<void(const QStringList& newf, const QStringList& modifiedf, const QStringList& deletedf)>;

    // 执行 git status命令，执行完毕后执行回调，传入改变的文件相对路径。
    // 注意：执行git status前会执行git add -A添加所有文件。
    void status(const GitStatusCallback &cb);

    const QString &roorDirPath() const;

    void all_file(QStringList *dirs, QStringList *files) const;
signals:
public slots:
private:
    const GitPrivate *d;
};

extern void __go();
#endif // GIT_H
