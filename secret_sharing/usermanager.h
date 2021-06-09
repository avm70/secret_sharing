#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <QtSql>

class UserManager : public QObject
{
    Q_OBJECT
public:
    explicit UserManager( QObject * parent = 0 );
public slots:
    bool haveUser( const QString & name );
    void addUser( const QString & name, const QString & passwd );
    bool loginUser( const QString & name, const QString & passwd );
    QList<QString> getUsers();
private:
    QMap<QString, QString> users;
    QSqlDatabase mDataBase;
    QMutex mMutex;
};

#endif // USERMANAGER_H
