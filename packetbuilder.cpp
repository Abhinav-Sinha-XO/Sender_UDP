#include "packetbuilder.h"

#include <QtGlobal>
#include <cmath>

static void appendUInt16(QByteArray &packet, quint16 value)
{
    packet.append(static_cast<char>((value >> 8) & 0xFF));
    packet.append(static_cast<char>(value & 0xFF));
}

static void appendUInt32(QByteArray &packet, quint32 value)
{
    packet.append(static_cast<char>((value >> 24) & 0xFF));
    packet.append(static_cast<char>((value >> 16) & 0xFF));
    packet.append(static_cast<char>((value >> 8) & 0xFF));
    packet.append(static_cast<char>(value & 0xFF));
}

quint16 PacketBuilder::calculateCrc16(const QByteArray &data)
{
    quint16 crc = 0xFFFF;

    for (char byte : data) {
        crc ^= static_cast<quint16>(
                   static_cast<quint8>(byte)
                   ) << 8;

        for (int bit = 0; bit < 8; ++bit) {
            if (crc & 0x8000) {
                crc = static_cast<quint16>(
                    (crc << 1) ^ 0x1021
                    );
            } else {
                crc = static_cast<quint16>(crc << 1);
            }
        }
    }

    return crc;
}

QByteArray PacketBuilder::buildPacketA(
    double speedMps,
    double bearingDegrees,
    quint8 serialization)
{
    QByteArray packet;
    packet.reserve(10);

    // Byte 0: Header
    packet.append(static_cast<char>(0xAA));

    // Byte 1: Packet ID for Packet A
    packet.append(static_cast<char>(0x01));

    // Byte 2: Serialization / sequence number
    packet.append(static_cast<char>(serialization));

    const quint16 speed = static_cast<quint16>(
        std::lround(speedMps * 10.0)
        );

    const quint16 bearing = static_cast<quint16>(
        std::lround(bearingDegrees * 100.0)
        );

    appendUInt16(packet, speed);
    appendUInt16(packet, bearing);

    // Bytes 7-8: Blank/zero CRC bytes (0x00, 0x00)
    packet.append(static_cast<char>(0x00));
    packet.append(static_cast<char>(0x00));

    // Byte 9: Footer
    packet.append(static_cast<char>(0x55));

    return packet;
}

QByteArray PacketBuilder::buildPacketB(
    double temperatureC,
    int equipmentStatus,
    double batteryVoltage,
    quint8 serialization)
{
    QByteArray packet;
    packet.reserve(10);

    // Byte 0: Header
    packet.append(static_cast<char>(0xAA));

    // Byte 1: Packet ID for Packet B
    packet.append(static_cast<char>(0x02));

    // Byte 2: Serialization / sequence number
    packet.append(static_cast<char>(serialization));

    const quint32 temperature = static_cast<quint32>(
        std::lround(temperatureC * 10.0)
        );

    const quint32 battery = static_cast<quint32>(
        std::lround(batteryVoltage * 10.0)
        );

    const quint32 payload =
        ((temperature & 0xFFFU) << 20) |
        ((static_cast<quint32>(equipmentStatus) & 0xFFU) << 12) |
        (battery & 0xFFFU);

    appendUInt32(packet, payload);

    // Bytes 7-8: Blank/zero CRC bytes (0x00, 0x00)
    packet.append(static_cast<char>(0x00));
    packet.append(static_cast<char>(0x00));

    // Byte 9: Footer
    packet.append(static_cast<char>(0x55));

    return packet;
}