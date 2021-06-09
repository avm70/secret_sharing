#include "user.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <iostream>

User::User( QTcpSocket * socket )
{
    mSocket = socket;
    connect( mSocket, &QTcpSocket::readyRead, this, &User::read );
    connect( mSocket, &QTcpSocket::disconnected, this, &User::close );
}

quint64 User::bytesAvailable()
{
    return mSocket->bytesAvailable();
}

QByteArray User::readAll()
{
    return mSocket->readAll();
}

bool User::isLogin()
{
    return isLogged;
}

void User::succesLogin()
{
    isLogged = true;
}

void User::write( const char * message )
{
    mSocket->write( message );
}

void User::write( const QByteArray & array )
{
    mSocket->write( array );
}

void User::read()
{
    if( inputLogin )
    {
        inputLogin = false;
        emit inLogin();
    }
    else if( inputPassword )
    {
        inputPassword = false;
        emit inPassword();
    }
    else if( registerLogin )
    {
        registerLogin = false;
        emit regLogin();
    }
    else if( registerPassword )
    {
        registerPassword = false;
        emit regPassword();
    }
    else if( storeSecret )
    {
        storeSecret = false;
        emit inSecret();
    }
    else if( sentSecret )
    {
        sentSecret = false;
        emit shareSecret();
    }
    else
    {
        emit readyRead();
    }
}

void User::close()
{
    mSocket->close();
    mSocket->deleteLater();
    emit disconnected();
}

void User::getPassword( const char * message )
{
    inputPassword = true;
    mSocket->write( message );
}

void User::getLogin( const char * message )
{
    inputLogin = true;
    mSocket->write( message );
}

void User::getRegPassword( const char * message )
{
    registerPassword = true;
    mSocket->write( message );
}

void User::getRegLogin( const char * message )
{
    registerLogin = true;
    mSocket->write( message );
}

void User::getSecret( const char * message )
{
    storeSecret = true;
    mSocket->write( message );
}

void User::sendSecret( const char * message )
{
    sentSecret = true;
    mSocket->write( message );
}
