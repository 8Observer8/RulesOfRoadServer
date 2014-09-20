
#include <QStringList>
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_server = new Server;
    connect( m_server, SIGNAL( signalReceivedData( QString ) ),
             this, SLOT( slotReceivedData( QString ) ) );
    connect( m_server, SIGNAL( signalError( QString ) ),
             this, SLOT( slotShowError( QString ) ) );
    connect( m_server, SIGNAL( signalStatusText( QString ) ),
             this, SLOT( slotSetStatusText( QString ) ) );

    statusLabel = new QLabel( this );
    ui->statusBar->addPermanentWidget( statusLabel );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotReceivedData( const QString &text )
{
    if ( text == QString( "Ok" ) ) {
        m_server->send( "Ok" );
        return;
    }

    QStringList list = text.split( "," );

    if ( list.size() != 3 ) {
        return;
    } else {
        foreach (QString s, list) {
            ui->outputTextEdit->append( s );
        }
        ui->outputTextEdit->append( "\n" );
    }
}

void MainWindow::slotSetStatusText( const QString &text )
{
    statusLabel->setText( text );
}

void MainWindow::slotShowError( const QString &errorText )
{
    statusLabel->setText( errorText );
}
