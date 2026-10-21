#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QString>
#include <QtGlobal>

class QLineEdit;
class QPushButton;
class QTextEdit;
class QLabel;
class QUdpSocket;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QLineEdit *ipInput;
    QLineEdit *portInput;

    QLineEdit *speedInput;
    QLineEdit *bearingInput;

    QLineEdit *temperatureInput;
    QLineEdit *statusInput;
    QLineEdit *batteryInput;

    QPushButton *configureButton;
    QPushButton *sendAButton;
    QPushButton *sendBButton;
    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *sendCorruptedButton;

    QTextEdit *logOutput;
    QLabel *statusLabel;

    QUdpSocket *udpSocket;
    QTimer *timerA;
    QTimer *timerB;

    bool configureDestination();
    bool sendUdpPacket(const QByteArray &packet,
                       const QString &packetName);

    void sendPacketA();
    void sendPacketB();
    void sendCorruptedPacket();

    void startAutomaticSending();
    void stopAutomaticSending();

    void addLog(const QString &message);

    quint8 globalSequence = 1;
};

#endif