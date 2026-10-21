
#ifndef PACKETBUILDER_H
#define PACKETBUILDER_H

#include <QByteArray>
#include <QtGlobal>

class PacketBuilder
{
public:
    static QByteArray buildPacketA(
        double speedMps,
        double bearingDegrees,
        quint8 serialization
        );

    static QByteArray buildPacketB(
        double temperatureC,
        int equipmentStatus,
        double batteryVoltage,
        quint8 serialization
        );

private:
    static quint16 calculateCrc16(const QByteArray &data);
};

#endif // PACKETBUILDER_H
