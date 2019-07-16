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
    ~Git();

    // 执行git status后的回调函数类型。入参为：新增的文件列表、修改的文件列表、已删除的文件列表
    using GitStatusCallback = std::function<void(const QStringList& newf, const QStringList& modifiedf, const QStringList& deletedf)>;

    // 执行 git status命令，执行完毕后执行回调，传入改变的文件相对路径。
    // 注意：执行git status前会执行git add -A添加所有文件。
    void status(const GitStatusCallback &cb);

    const QString &roorDirPath() const;

    // 遍历所有满足.gitignore规则的文件和目录。、
    void allFiles(QStringList *dirs, QStringList *files) const;

    bool isIgnored(const QString &path) const;

    // git add -A，把指定目录里的文件加入repo
    void addAll(const QString &path) const;

    void lastError(int error) const;
private:
    const GitPrivate *d;
};


#endif // GIT_H
