
#include <QNetworkConfigurationManager>
#include <QSettings>
#include "Server.h"

Server::Server() :
    m_tcpServer( 0 ),
    m_networkSession( 0 ),
    m_nextBlockSize( 0 )
{
    configurateNetwork();
    connect( m_tcpServer, SIGNAL( newConnection() ),
             this, SLOT( slotNewConnection() ) );
}

void Server::send( const QString &text )
{
    QByteArray  arrBlock;
    QDataStream out( &arrBlock, QIODevice::WriteOnly );
    out.setVersion( QDataStream::Qt_5_0 );

    out << quint16( 0 ) << text;

    out.device()->seek( 0 );
    out << quint16( arrBlock.size() - sizeof( quint16 ) );

    m_socket->write( arrBlock );
}

void Server::configurateNetwork()
{
    QNetworkConfigurationManager manager;
    if ( manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired ) {
        // Get saved network configuration
        QSettings settings( QSettings::UserScope, QLatin1String( "QtProject" ) );
        settings.beginGroup( QLatin1String( "QtNetwork" ) );
        const QString id = settings.value( QLatin1String( "DefaultNetworkConfiguration" ) ).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier( id );
        if ( ( config.state() & QNetworkConfiguration::Discovered ) !=
                QNetworkConfiguration::Discovered ) {
            config = manager.defaultConfiguration();
        }

        m_networkSession = new QNetworkSession( config, this );
        connect( m_networkSession, SIGNAL( opened() ), this, SLOT( sessionOpened() ) );

        //statusLabel->setText(tr("Opening network session."));
        m_networkSession->open();
    } else {
        slotSessionOpened();
    }
}

void Server::slotSessionOpened()
{
    // Save the used configuration
    if ( m_networkSession ) {
        QNetworkConfiguration config = m_networkSession->configuration();
        QString id;
        if ( config.type() == QNetworkConfiguration::UserChoice ) {
            id = m_networkSession->sessionProperty( QLatin1String( "UserChoiceConfiguration" ) ).toString();
        }
        else {
            id = config.identifier();
        }

        QSettings settings( QSettings::UserScope, QLatin1String( "QtProject" ) );
        settings.beginGroup( QLatin1String( "QtNetwork" ) );
        settings.setValue( QLatin1String( "DefaultNetworkConfiguration" ), id );
        settings.endGroup();
    }

    m_tcpServer = new QTcpServer(this);
    if ( !m_tcpServer->listen( QHostAddress::Any, 1234 ) ) {
        QString message = tr( "Unable to start the server: %1").
                              arg( m_tcpServer->errorString() );
        emit signalError( message );
        return;
    }

//    QString ipAddress;
//    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
//    // use the first non-localhost IPv4 address
//    for (int i = 0; i < ipAddressesList.size(); ++i) {
//        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
//                ipAddressesList.at(i).toIPv4Address()) {
//            ipAddress = ipAddressesList.at(i).toString();
//            break;
//        }
//    }

//    // if we did not find one, use IPv4 localhost
//    if (ipAddress.isEmpty()) {
//        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    //    }
}

void Server::slotNewConnection()
{
    m_socket = m_tcpServer->nextPendingConnection();
    connect(m_socket, SIGNAL( disconnected() ),
            m_socket, SLOT( deleteLater() )
            );
    connect( m_socket, SIGNAL( readyRead() ),
            this, SLOT( slotReadyRead() )
             );
    emit signalStatusText( tr( "Someone connected" ) );
}

void Server::slotReadyRead()
{
    QTcpSocket* clientSocket = ( QTcpSocket* )sender();
    QDataStream in( clientSocket );
    in.setVersion( QDataStream::Qt_5_0 );

    for ( ;; ) {
        if (!m_nextBlockSize) {
            if (clientSocket->bytesAvailable() < (int)sizeof(quint16)) {
                break;
            }
            in >> m_nextBlockSize;
        }

        if (clientSocket->bytesAvailable() < m_nextBlockSize) {
            break;
        }

        QString inputData;
        in >> inputData;
        emit signalReceivedData( inputData );

        m_nextBlockSize = 0;
    }
}
