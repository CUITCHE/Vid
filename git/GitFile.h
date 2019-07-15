#ifndef GITFILE_H
#define GITFILE_H

#include <QString>
#include <functional>

struct GitFilePrivate;

class GitFile
{
public:
    using FilePathCallBack = std::function<void(const QString &filepath)>;
    using DirCallBack = std::function<void(const QString &path)>;
    GitFile(const QString &root);
    ~GitFile();

    void add(const QString &filename, const QString &root);
    void addPath(const QString &path, const QString &root);

    void visitFiles(const FilePathCallBack &cb) const;
    void visitPath(const DirCallBack &cb) const;
private:
    GitFilePrivate *d;
};

#endif // GITFILE_H
