#ifndef USER_H
#define USER_H

#include <QObject>
#include <QTcpSocket>

class User : public QObject
{
    Q_OBJECT
public:
    User( QTcpSocket * socket );
    quint64 bytesAvailable();
    QByteArray readAll();
    bool isLogin();
    void write( const QByteArray & array );
    void write( const char *message );
    void read();
    void close();
    void succesLogin();
    QString name;
    void getPassword( const char * message );
    void getLogin( const char * message );
    void getRegPassword( const char * message );
    void getRegLogin( const char * message );
    void getSecret( const char * message );
    void sendSecret( const char * message );
private:
    bool isLogged = false;
    QTcpSocket * mSocket;
    bool inputLogin = false;
    bool inputPassword = false;
    bool registerPassword = false;
    bool registerLogin = false;
    bool storeSecret = false;
    bool sentSecret = false;
signals:
    void regLogin();
    void regPassword();
    void inLogin();
    void inPassword();
    void inSecret();
    void readyRead();
    void disconnected();
    void getSecrets();
    void shareSecret();
};

#endif // USER_H
