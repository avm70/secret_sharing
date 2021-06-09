#include "usermanager.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QCryptographicHash>
#include <iostream>

UserManager::UserManager( QObject * parent ) : QObject( parent )
{
    QDir dir = QDir();
    dir.mkdir( "db" );
    mDataBase = QSqlDatabase::addDatabase( "QSQLITE" );
    mDataBase.setDatabaseName( "db/users.db" );
    if( mDataBase.open() )
    {
        if( mDataBase.tables().contains( QLatin1String( "users" ) ) )
        {
            QSqlQuery query( "SELECT * FROM users", mDataBase );
            query.exec();
            int idName = query.record().indexOf( "name" );
            int idPasswd = query.record().indexOf( "password" );
            while( query.next() )
            {
                QString name = query.value( idName ).toString();
                QString passwd = query.value( idPasswd ).toString();
                users.insert( name, passwd );
            }
        }
        else
        {
            QSqlQuery query( "CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, password TEXT)", mDataBase );
            query.exec();
        }
    }
    else
    {
        qDebug() << "database not open " << mDataBase.lastError().text();
    }
}

bool UserManager::haveUser( const QString & name )
{
    return users.contains( name );
}

void UserManager::addUser( const QString & name, const QString & passwd )
{
    QMutexLocker guard( &mMutex );
    users.insert( name, passwd );
    guard.unlock();
    QSqlQuery query( mDataBase );
    query.prepare( "INSERT INTO users (name, password) VALUES (:name, :password)" );
    query.bindValue( ":name", name );
    query.bindValue( ":password", passwd );
    if( query.exec() )
    {
       bool success = true;
    }
    else
    {
        qDebug() << "add user error:"
                     << query.lastError();
    }
}

bool UserManager::loginUser( const QString & name, const QString & passwd )
{
    QMutexLocker guard( &mMutex );
    return users.value( name ) == passwd;
}

QList<QString> UserManager::getUsers()
{
    QMutexLocker guard( &mMutex );
    return users.keys();
}
