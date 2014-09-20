#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "Server.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slotReceivedData( const QString &text );
    void slotSetStatusText( const QString &text );
    void slotShowError( const QString &errorText );

private:
    Ui::MainWindow *ui;
    Server *m_server;
    QLabel *statusLabel;
};

#endif // MAINWINDOW_H
