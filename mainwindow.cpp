
#include "mainwindow.h"
#include "packetbuilder.h"

#include <QAbstractSocket>
#include <QDoubleValidator>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QTextEdit>
#include <QTimer>
#include <QUdpSocket>
#include <QVBoxLayout>
#include <QWidget>

// ============================================================
// CONSTRUCTOR
// ============================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ipInput(new QLineEdit),
    portInput(new QLineEdit),
    speedInput(new QLineEdit),
    bearingInput(new QLineEdit),
    temperatureInput(new QLineEdit),
    statusInput(new QLineEdit),
    batteryInput(new QLineEdit),
    configureButton(new QPushButton("Apply Destination")),
    sendAButton(new QPushButton("Send Packet A")),
    sendBButton(new QPushButton("Send Packet B")),
    startButton(new QPushButton("Start Automatic Sending")),
    stopButton(new QPushButton("Stop Automatic Sending")),
    logOutput(new QTextEdit),
    statusLabel(new QLabel("Ready")),
    udpSocket(new QUdpSocket(this)),
    timerA(new QTimer(this)),
    timerB(new QTimer(this))
{
    setWindowTitle("UDP Packet Simulator | Team A");
    resize(1000, 900);
    setMinimumSize(650, 600);

    // ========================================================
    // PURE BLACK THEME
    // ========================================================

    setStyleSheet(R"(
        QMainWindow {
            background-color: #000000;
            color: #FFFFFF;
        }

        QWidget {
            background-color: #000000;
            color: #FFFFFF;
            font-family: "Segoe UI";
            font-size: 10pt;
        }

        QScrollArea {
            border: none;
            background-color: #000000;
        }

        QGroupBox {
            background-color: #080808;
            border: 1px solid #292929;
            border-radius: 8px;
            margin-top: 18px;
            padding: 18px;
            font-weight: 600;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            left: 12px;
            padding: 0 8px;
            color: #70B7FF;
            background-color: #000000;
        }

        QLabel {
            background-color: transparent;
            color: #EAEAEA;
        }

        QLineEdit {
            background-color: #050505;
            color: #FFFFFF;
            border: 1px solid #303030;
            border-radius: 5px;
            padding: 8px 10px;
            min-height: 20px;
            selection-background-color: #1677FF;
            selection-color: #FFFFFF;
        }

        QLineEdit:focus {
            border: 1px solid #1677FF;
        }

        QPushButton {
            background-color: #1677FF;
            color: #FFFFFF;
            border: 1px solid #1677FF;
            border-radius: 6px;
            padding: 10px 16px;
            min-height: 22px;
            font-weight: 600;
        }

        QPushButton:hover {
            background-color: #3289FF;
        }

        QPushButton:pressed {
            background-color: #095BCB;
        }

        QPushButton:disabled {
            background-color: #171717;
            color: #686868;
            border-color: #292929;
        }

        QTextEdit {
            background-color: #050505;
            color: #D8E5F5;
            border: 1px solid #292929;
            border-radius: 6px;
            padding: 10px;
            font-family: Consolas, monospace;
            font-size: 10pt;
        }

        QScrollBar:vertical {
            background-color: #080808;
            width: 10px;
            margin: 0;
        }

        QScrollBar::handle:vertical {
            background-color: #383838;
            border-radius: 4px;
            min-height: 25px;
        }

        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0;
        }
    )");

    // ========================================================
    // SCROLLABLE MAIN WINDOW CONTENT
    // ========================================================

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QScrollArea::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded);

    auto *content = new QWidget;
    content->setObjectName("mainContent");

    auto *mainLayout = new QVBoxLayout(content);
    mainLayout->setSpacing(18);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // ========================================================
    // HEADER
    // ========================================================

    auto *heading = new QLabel("UDP PACKET SIMULATOR");

    heading->setStyleSheet(
        "font-size: 25pt;"
        "font-weight: 700;"
        "color: #70B7FF;"
        "background-color: transparent;"
        );

    mainLayout->addWidget(heading);

    auto *subtitle = new QLabel(
        "TEAM A  |  Binary UDP packets  |  10 bytes each");

    subtitle->setStyleSheet(
        "color: #999999;"
        "background-color: transparent;"
        );

    mainLayout->addWidget(subtitle);

    // Consistent sizing for input fields.
    const int inputWidth = 280;

    ipInput->setMinimumWidth(inputWidth);
    portInput->setMinimumWidth(inputWidth);
    speedInput->setMinimumWidth(inputWidth);
    bearingInput->setMinimumWidth(inputWidth);
    temperatureInput->setMinimumWidth(inputWidth);
    statusInput->setMinimumWidth(inputWidth);
    batteryInput->setMinimumWidth(inputWidth);

    // ========================================================
    // UDP DESTINATION
    // ========================================================

    auto *destinationGroup =
        new QGroupBox("UDP DESTINATION");

    auto *destinationLayout =
        new QFormLayout(destinationGroup);

    destinationLayout->setVerticalSpacing(14);
    destinationLayout->setHorizontalSpacing(18);
    destinationLayout->setContentsMargins(18, 24, 18, 18);
    destinationLayout->setLabelAlignment(Qt::AlignLeft);
    destinationLayout->setFieldGrowthPolicy(
        QFormLayout::AllNonFixedFieldsGrow);

    ipInput->setPlaceholderText("Receiver IPv4 address");
    ipInput->setText("127.0.0.1");

    portInput->setPlaceholderText("UDP port");
    portInput->setText("5000");

    portInput->setValidator(
        new QIntValidator(1, 65535, portInput));

    destinationLayout->addRow("Receiver IP:", ipInput);
    destinationLayout->addRow("Receiver port:", portInput);

    auto *destinationButtonRow = new QHBoxLayout;
    destinationButtonRow->setSpacing(12);
    destinationButtonRow->addWidget(configureButton);
    destinationButtonRow->addStretch();

    destinationLayout->addRow("", destinationButtonRow);

    mainLayout->addWidget(destinationGroup);

    // ========================================================
    // PACKET A
    // ========================================================

    auto *packetAGroup =
        new QGroupBox("PACKET A  |  SPEED AND BEARING");

    auto *packetALayout =
        new QFormLayout(packetAGroup);

    packetALayout->setVerticalSpacing(14);
    packetALayout->setHorizontalSpacing(18);
    packetALayout->setContentsMargins(18, 24, 18, 18);
    packetALayout->setLabelAlignment(Qt::AlignLeft);
    packetALayout->setFieldGrowthPolicy(
        QFormLayout::AllNonFixedFieldsGrow);

    speedInput->setText("25.5");
    bearingInput->setText("120.0");

    auto *speedValidator =
        new QDoubleValidator(0.0, 6553.5, 3, speedInput);

    speedValidator->setNotation(
        QDoubleValidator::StandardNotation);
    speedValidator->setLocale(QLocale::c());

    speedInput->setValidator(speedValidator);

    auto *bearingValidator =
        new QDoubleValidator(0.0, 359.99, 2, bearingInput);

    bearingValidator->setNotation(
        QDoubleValidator::StandardNotation);
    bearingValidator->setLocale(QLocale::c());

    bearingInput->setValidator(bearingValidator);

    packetALayout->addRow("Speed (m/s):", speedInput);
    packetALayout->addRow("Bearing (degrees):", bearingInput);

    auto *packetAButtonRow = new QHBoxLayout;
    packetAButtonRow->setSpacing(12);
    packetAButtonRow->addWidget(sendAButton);
    packetAButtonRow->addStretch();

    packetALayout->addRow("", packetAButtonRow);

    mainLayout->addWidget(packetAGroup);

    // ========================================================
    // PACKET B
    // ========================================================

    auto *packetBGroup =
        new QGroupBox(
            "PACKET B  |  TEMPERATURE, STATUS AND BATTERY");

    auto *packetBLayout =
        new QFormLayout(packetBGroup);

    packetBLayout->setVerticalSpacing(14);
    packetBLayout->setHorizontalSpacing(18);
    packetBLayout->setContentsMargins(18, 24, 18, 18);
    packetBLayout->setLabelAlignment(Qt::AlignLeft);
    packetBLayout->setFieldGrowthPolicy(
        QFormLayout::AllNonFixedFieldsGrow);

    temperatureInput->setText("36.5");
    statusInput->setText("1");
    batteryInput->setText("12.4");

    auto *temperatureValidator =
        new QDoubleValidator(
            0.0, 409.5, 1, temperatureInput);

    temperatureValidator->setNotation(
        QDoubleValidator::StandardNotation);
    temperatureValidator->setLocale(QLocale::c());

    temperatureInput->setValidator(temperatureValidator);

    auto *statusValidator =
        new QIntValidator(0, 255, statusInput);

    statusInput->setValidator(statusValidator);

    auto *batteryValidator =
        new QDoubleValidator(
            0.0, 409.5, 1, batteryInput);

    batteryValidator->setNotation(
        QDoubleValidator::StandardNotation);
    batteryValidator->setLocale(QLocale::c());

    batteryInput->setValidator(batteryValidator);

    packetBLayout->addRow(
        "Temperature (°C):", temperatureInput);

    packetBLayout->addRow(
        "Equipment status (0–255):", statusInput);

    packetBLayout->addRow(
        "Battery voltage (V):", batteryInput);

    auto *packetBButtonRow = new QHBoxLayout;
    packetBButtonRow->setSpacing(12);
    packetBButtonRow->addWidget(sendBButton);
    packetBButtonRow->addStretch();

    packetBLayout->addRow("", packetBButtonRow);

    mainLayout->addWidget(packetBGroup);

    // ========================================================
    // AUTOMATIC SENDING
    // ========================================================

    auto *timerGroup =
        new QGroupBox("AUTOMATIC SENDING");

    auto *timerLayout = new QVBoxLayout(timerGroup);
    timerLayout->setSpacing(16);
    timerLayout->setContentsMargins(18, 24, 18, 18);

    auto *timerInfo = new QLabel(
        "Packet A: every 30 seconds"
        "     |     "
        "Packet B: every 60 seconds");

    timerInfo->setStyleSheet(
        "color: #AAAAAA;"
        "background-color: transparent;"
        );

    timerLayout->addWidget(timerInfo);

    auto *timerButtons = new QHBoxLayout;
    timerButtons->setSpacing(12);

    startButton->setMinimumWidth(200);
    stopButton->setMinimumWidth(200);

    timerButtons->addWidget(startButton);
    timerButtons->addWidget(stopButton);

    timerLayout->addLayout(timerButtons);
    mainLayout->addWidget(timerGroup);

    // ========================================================
    // STATUS
    // ========================================================

    statusLabel->setStyleSheet(
        "color: #6EE7A8;"
        "font-weight: bold;"
        "padding: 4px;"
        "background-color: transparent;"
        );

    mainLayout->addWidget(statusLabel);

    // ========================================================
    // TRANSMISSION LOG
    // ========================================================

    auto *logHeading = new QLabel("TRANSMISSION LOG");

    logHeading->setStyleSheet(
        "font-size: 13pt;"
        "font-weight: 700;"
        "color: #70B7FF;"
        "background-color: transparent;"
        );

    mainLayout->addWidget(logHeading);

    logOutput->setReadOnly(true);
    logOutput->setMinimumHeight(220);
    logOutput->setPlaceholderText(
        "Packet transmission details will appear here...");

    mainLayout->addWidget(logOutput);

    // Add space after the log.
    mainLayout->addSpacing(10);

    scrollArea->setWidget(content);
    setCentralWidget(scrollArea);

    // ========================================================
    // TIMERS
    // ========================================================

    timerA->setInterval(30000);
    timerB->setInterval(60000);

    // ========================================================
    // SIGNAL CONNECTIONS
    // ========================================================

    connect(configureButton, &QPushButton::clicked,
            this, [this]() {
                if (configureDestination()) {
                    statusLabel->setText("Destination configured");
                    addLog("Destination settings validated.");
                }
            });

    connect(sendAButton, &QPushButton::clicked,
            this, &MainWindow::sendPacketA);

    connect(sendBButton, &QPushButton::clicked,
            this, &MainWindow::sendPacketB);

    connect(startButton, &QPushButton::clicked,
            this, &MainWindow::startAutomaticSending);

    connect(stopButton, &QPushButton::clicked,
            this, &MainWindow::stopAutomaticSending);

    connect(timerA, &QTimer::timeout,
            this, &MainWindow::sendPacketA);

    connect(timerB, &QTimer::timeout,
            this, &MainWindow::sendPacketB);

    stopButton->setEnabled(false);

    // ========================================================
    // INITIAL LOG
    // ========================================================

    addLog("Application started.");
    addLog("Packet A format: 10 bytes.");
    addLog("Packet B format: 10 bytes.");
    addLog("CRC: CRC-16/CCITT-FALSE.");
    addLog("Automatic sending is stopped.");
}

// ============================================================
// VALIDATE DESTINATION
// ============================================================

bool MainWindow::configureDestination()
{
    QHostAddress address;
    const QString ip = ipInput->text().trimmed();

    bool portOk = false;
    const int port = portInput->text().toInt(&portOk);

    if (!address.setAddress(ip) ||
        address.protocol() != QAbstractSocket::IPv4Protocol)
    {
        QMessageBox::warning(
            this,
            "Invalid IP Address",
            "Enter a valid IPv4 address, for example "
            "192.168.1.20.");

        return false;
    }

    if (!portOk || port < 1 || port > 65535)
    {
        QMessageBox::warning(
            this,
            "Invalid UDP Port",
            "Enter a port between 1 and 65535.");

        return false;
    }

    return true;
}

// ============================================================
// SEND UDP PACKET
// ============================================================

bool MainWindow::sendUdpPacket(
    const QByteArray &packet,
    const QString &packetName)
{
    if (!configureDestination())
        return false;

    if (packet.size() != 10)
    {
        addLog(
            packetName +
            ": ERROR — packet must be exactly 10 bytes.");

        statusLabel->setText("Packet construction failed");
        return false;
    }

    QHostAddress destination;

    if (!destination.setAddress(ipInput->text().trimmed()))
    {
        addLog("Invalid destination IP.");
        return false;
    }

    bool portOk = false;
    const uint portValue =
        portInput->text().toUInt(&portOk);

    if (!portOk || portValue < 1 || portValue > 65535)
    {
        addLog("Invalid UDP port.");
        return false;
    }

    const quint16 port =
        static_cast<quint16>(portValue);

    const qint64 sentBytes =
        udpSocket->writeDatagram(
            packet, destination, port);

    if (sentBytes != packet.size())
    {
        addLog(
            packetName + ": UDP send failed: " +
            udpSocket->errorString());

        statusLabel->setText("UDP send failed");
        return false;
    }

    addLog(
        packetName + " submitted: " +
        QString::number(sentBytes) + " bytes");

    addLog(
        "HEX: " +
        QString::fromLatin1(
            packet.toHex(' ').toUpper()));

    addLog(
        "Destination: " +
        destination.toString() + ":" +
        QString::number(port));

    addLog("----------------------------------------");

    statusLabel->setText(
        packetName + " submitted to UDP socket");

    return true;
}

// ============================================================
// SEND PACKET A
// ============================================================


void MainWindow::sendPacketA()
{
    bool speedOk = false;
    bool bearingOk = false;

    const double speed =
        speedInput->text().toDouble(&speedOk);

    const double bearing =
        bearingInput->text().toDouble(&bearingOk);

    if (!speedOk || !bearingOk)
    {
        addLog("Packet A: enter valid speed and bearing values.");
        return;
    }

    if (speed < 0.0 || speed > 6553.5 ||
        bearing < 0.0 || bearing >= 360.0)
    {
        addLog(
            "Packet A: speed must be 0–6553.5 m/s "
            "and bearing must be 0–359.99 degrees.");
        return;
    }

    // Build the packet using the current serialization value.
    const QByteArray packet =
        PacketBuilder::buildPacketA(
            speed,
            bearing,
            packetASequence);

    if (packet.size() != 10)
    {
        addLog("Packet A construction failed.");
        return;
    }

    // Increment only after the UDP send succeeds.
    const quint8 sentSequence = packetASequence;

    if (sendUdpPacket(packet, "PACKET A"))
    {
        addLog(
            "Packet A serialization sent: " +
            QString::number(sentSequence));

        packetASequence =
            static_cast<quint8>(packetASequence + 1);
    }
}


// ============================================================
// SEND PACKET B
// ============================================================


void MainWindow::sendPacketB()
{
    bool temperatureOk = false;
    bool statusOk = false;
    bool batteryOk = false;

    const double temperature =
        temperatureInput->text().toDouble(&temperatureOk);

    const int equipmentStatus =
        statusInput->text().toInt(&statusOk);

    const double battery =
        batteryInput->text().toDouble(&batteryOk);

    if (!temperatureOk || !statusOk || !batteryOk)
    {
        addLog(
            "Packet B: enter valid temperature, status "
            "and battery values.");
        return;
    }

    if (temperature < 0.0 || temperature > 409.5 ||
        equipmentStatus < 0 || equipmentStatus > 255 ||
        battery < 0.0 || battery > 409.5)
    {
        addLog(
            "Packet B: temperature and battery must be "
            "0–409.5; status must be 0–255.");
        return;
    }

    // Build the packet using the current serialization value.
    const QByteArray packet =
        PacketBuilder::buildPacketB(
            temperature,
            equipmentStatus,
            battery,
            packetBSequence);

    if (packet.size() != 10)
    {
        addLog("Packet B construction failed.");
        return;
    }

    // Increment only after the UDP send succeeds.
    const quint8 sentSequence = packetBSequence;

    if (sendUdpPacket(packet, "PACKET B"))
    {
        addLog(
            "Packet B serialization sent: " +
            QString::number(sentSequence));

        packetBSequence =
            static_cast<quint8>(packetBSequence + 1);
    }
}


// ============================================================
// START AUTOMATIC SENDING
// ============================================================

void MainWindow::startAutomaticSending()
{
    if (!configureDestination())
        return;

    timerA->start();
    timerB->start();

    startButton->setEnabled(false);
    stopButton->setEnabled(true);

    statusLabel->setText(
        "Automatic sending active: A = 30 s, B = 60 s");

    addLog("Automatic sending started.");
    addLog("Packet A interval: 30 seconds.");
    addLog("Packet B interval: 60 seconds.");
    addLog("First sends occur after their timer intervals.");
}

// ============================================================
// STOP AUTOMATIC SENDING
// ============================================================

void MainWindow::stopAutomaticSending()
{
    timerA->stop();
    timerB->stop();

    startButton->setEnabled(true);
    stopButton->setEnabled(false);

    statusLabel->setText("Automatic sending stopped");

    addLog("Automatic sending stopped.");
}

// ============================================================
// LOGGING
// ============================================================

void MainWindow::addLog(const QString &message)
{
    logOutput->append(message);

    logOutput->verticalScrollBar()->setValue(
        logOutput->verticalScrollBar()->maximum());
}
