#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QNetworkSession>
#include <QTcpServer>
#include <QTcpSocket>

class Server : public QObject
{
    Q_OBJECT

public:
    Server();

    void send( const QString &text );

signals:
    void signalReceivedData( const QString &text );
    void signalError( const QString &errorText );
    void signalStatusText( const QString &statusText );

private:
    void configurateNetwork();

private slots:
    void slotSessionOpened();
    void slotNewConnection();
    void slotReadyRead();

private:
    QNetworkSession *m_networkSession;
    QTcpServer *m_tcpServer;
    QTcpSocket *m_socket;
    quint16 m_nextBlockSize;
};

#endif // SERVER_H
