#include "tcpserver.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <iostream>

TcpServer::TcpServer( QObject * parent ) : QObject( parent )
{
    mTcpServer = new QTcpServer( this );
    connect(mTcpServer, &QTcpServer::newConnection, this, &TcpServer::slotNewConnection, Qt::QueuedConnection );
    if( !mTcpServer->listen( QHostAddress::AnyIPv4, 6000 ) )
        qDebug() << "server is not started";
    else
        qDebug() << "server is started";
    QDir dir = QDir();
    dir.mkdir( "secrets" );
}

void TcpServer::slotNewConnection()
{
    User * user = new User( mTcpServer->nextPendingConnection() );
    connect( user, &User::readyRead, this, &TcpServer::slotServerRead, Qt::QueuedConnection );
    connect( user, &User::disconnected, this, &TcpServer::slotClientDisconnected, Qt::QueuedConnection );
    connect( user, &User::inLogin, this, &TcpServer::readLogin, Qt::QueuedConnection );
    connect( user, &User::inPassword, this, &TcpServer::readPassword, Qt::QueuedConnection );
    connect( user, &User::regLogin, this, &TcpServer::readRegLogin, Qt::QueuedConnection );
    connect( user, &User::regPassword, this, &TcpServer::readRegPassword, Qt::QueuedConnection );
    connect( user, &User::inSecret, this, &TcpServer::readSecret, Qt::QueuedConnection );
    connect( user, &User::shareSecret, this, &TcpServer::sendSecret, Qt::QueuedConnection );
    user->write( "Wellcome\n1.To register\n2.To login\n>" );
    mUsers.append( user );
}

void TcpServer::readLogin()
{
    User * user = ( User * )QObject::sender();
    while( user->bytesAvailable() > 0 )
    {
        QByteArray array = user->readAll();
        QString message( array );
        message.remove("\n");
        if( !user->isLogin() )
        {
            if( mUserManager.haveUser( message ) )
            {
                user->name = message;
                user->getPassword( "Login. Input password: " );
            }
            else
            {
                user->write( "No such user.\n1.To register\n2.To login\n>" );
            }
        }
    }
}

void TcpServer::readPassword()
{
    User * user = ( User * )QObject::sender();
    while( user->bytesAvailable() > 0 )
    {
        QByteArray array = user->readAll();
        QString message( array );
        message.remove("\n");
        if( !user->isLogin() )
        {
            if( mUserManager.loginUser( user->name, message ) )
            {
                user->succesLogin();
                user->write( "Success login.\n1.To Show Your secrets\n2.To Store Secret.\n3.To Show Users\n4.Send secret\n>");
            }
            else
            {
                user->write( "Wrong password\n1.To register\n2.To login\n>");
            }
        }
    }
}

void TcpServer::readRegLogin()
{
    User * user = ( User * )QObject::sender();
    while( user->bytesAvailable() > 0 )
    {
        QByteArray array = user->readAll();
        QString message( array );
        message.remove("\n");
        if( mUserManager.haveUser( message ) )
        {
            user->write( "User already exist\n1.To register\n2.To login\n>" );
        }
        else
        {
            user->name = message;
            user->getRegPassword( "Register. Input password: " );
        }
    }
}

void TcpServer::readRegPassword()
{
    User * user = ( User * )QObject::sender();
    while( user->bytesAvailable() > 0 )
    {
        QByteArray array = user->readAll();
        QString message( array );
        message.remove("\n");
        mUserManager.addUser( user->name, message );
        user->write( "Success.\n1.To register\n2.To login\n>" );
    }
}

void TcpServer::readSecret()
{
    User * user = ( User * )QObject::sender();
    while( user->bytesAvailable() > 0 )
    {
        QByteArray array = user->readAll();
        QString message( array );
        message.remove("\n");
        QString dirname = "secrets/" + user->name.remove( '/' );
        QDir dir = QDir();
        dir.mkdir( dirname );
        QString filename = user->name + "_secrets.txt";
        QFile file( dirname + "/" + filename );
        if( file.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text ) )
        {
            file.write( message.toLocal8Bit() );
            file.write( QString("\n").toLocal8Bit() );
            file.close();
            user->write( "Secret saved\n1.To Show Your secrets\n2.To Store Secret.\n3.To Show Users\n4.Send secret\n>" );
        }
        else
        {
            user->write( "Unknown error\n1.To Show Your secrets\n2.To Store Secret.\n3.To Show Users\n4.Send secret\n>" );
        }
    }
}

void TcpServer::slotServerRead()
{
    User * user = ( User * )QObject::sender();
    if( user )
    while( user->bytesAvailable() > 0 )
    {
        QByteArray array = user->readAll();
        QString message( array );
        message.remove("\n");
        if( !user->isLogin() )
        {
            if( message == "1" )
            {
                user->getRegLogin( "Register. Input name: " );
            }
            else if( message == "2" )
            {
                user->getLogin( "Login. Input name: " );
            }
            else
            {
                user->write( "Unknown command\n>" );
            }
        }
        else
        {
            if( message == "1" )
            {
                showSecrets( user );
            }
            else if( message == "2" )
            {
                user->getSecret( "Input secret\n>" );
            }
            else if( message == "3" )
            {
                showUsers( user );
            }
            else if( message == "4" )
            {
                user->sendSecret( "Input user name\n>" );
            }
            else
            {
                user->write( "Unknown command\n1.To Show Your secrets\n2.To Store Secret.\n3.To Show Users\n4.Send secret\n>" );
            }
        }
    }
}

void TcpServer::slotClientDisconnected()
{
    User * user = ( User * )QObject::sender();
    mUsers.removeOne( user );
    user->deleteLater();
}

void TcpServer::showSecrets( User * user )
{
    QString dirname = "secrets/" + user->name;
    QDir dir(dirname + "/");
    if( dir.entryList( QDir::Files ).empty() )
    {
        user->write("No secrets\n1.To Show Your secrets\n2.To Store Secret.\n3.To Show Users\n4.Send secret\n>");
    }
    else
    {
        for( QString filename : dir.entryList( QDir::Files ) )
        {
            QFile file( dirname + "/" + filename );
            if( file.open( QIODevice::ReadOnly ) )
            {
               QTextStream in( &file );
               while( !in.atEnd() )
               {
                  user->write( in.readLine().toLocal8Bit() );
                  user->write( "\n" );
               }
               file.close();
            }
            else
            {
                user->write("No secrets\n1.To Show Your secrets\n2.To Store Secret.\n3.To Show Users\n4.Send secret\n>");
            }
        }
        user->write("1.To Show Your secrets\n2.To Store Secret.\n3.To Show Users\n4.Send secret\n>");
    }
}

void TcpServer::showUsers( User * user )
{
    foreach( const QString & name, mUserManager.getUsers() )
    {
        QString output = name + "\n";
        user->write( output.toLocal8Bit() );
    }
    user->write( "1.To Show Your secrets\n2.To Store Secret.\n3.To Show Users\n4.Send secret\n>" );
}

void TcpServer::sendSecret()
{
    User * user = ( User * )QObject::sender();
    while( user->bytesAvailable() > 0 )
    {
        QByteArray array = user->readAll();
        QString message( array );
        message.remove("\n");
        QString dirname = "secrets/" + user->name + "/";
        QString filename = user->name + "_secrets.txt";
        QProcess * process  = new QProcess();
        QString command = "cp " + dirname + filename + " secrets/" + message + "/";
        process->start( command );
        mProcess.insert( process, user );
        connect( process, SIGNAL(finished(int)), this, SLOT(finishProcess(int)), Qt::QueuedConnection );
    }
}

void TcpServer::finishProcess( int exitCode )
{
    QProcess * process = ( QProcess * )QObject::sender();
    User * user = mProcess.take( process );
    process->deleteLater();
    if( exitCode == 0 )
    {
        user->write( "Success secret send.\n1.To Show Your secrets\n2.To Store Secret.\n3.To Show Users\n4.Send secret\n>" );
    }
    else
    {
        user->write( "Error.\n1.To Show Your secrets\n2.To Store Secret.\n3.To Show Users\n4.Send secret\n>" );
    }
}
