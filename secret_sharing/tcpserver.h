#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtSql>
#include "usermanager.h"
#include "user.h"

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer( QObject * parent = 0 );
public slots:
    void slotNewConnection();
    void slotServerRead();
    void slotClientDisconnected();
    void showSecrets( User * user );
    void showUsers( User * user );
    void readLogin();
    void readPassword();
    void readRegLogin();
    void readRegPassword();
    void readSecret();
    void sendSecret();
    void finishProcess( int exitCode );
private:
    QMap<QProcess *, User*> mProcess;
    QTcpServer * mTcpServer;
    QList<User*> mUsers;
    UserManager mUserManager;
};

#endif // TCPSERVER_H
