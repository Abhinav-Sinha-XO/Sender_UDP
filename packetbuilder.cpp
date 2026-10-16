
#include "packetbuilder.h"

#include <QtGlobal>
#include <cmath>

// Append a 16-bit value in big-endian order.
static void appendUInt16(QByteArray &packet, quint16 value)
{
    packet.append(static_cast<char>((value >> 8) & 0xFF));
    packet.append(static_cast<char>(value & 0xFF));
}

// Append a 32-bit value in big-endian order.
static void appendUInt32(QByteArray &packet, quint32 value)
{
    packet.append(static_cast<char>((value >> 24) & 0xFF));
    packet.append(static_cast<char>((value >> 16) & 0xFF));
    packet.append(static_cast<char>((value >> 8) & 0xFF));
    packet.append(static_cast<char>(value & 0xFF));
}

// CRC-16/CCITT-FALSE:
// Polynomial: 0x1021
// Initial value: 0xFFFF
// XorOut: 0x0000
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

// Packet A payload:
// Bytes 3-4: speed multiplied by 10
// Bytes 5-6: bearing multiplied by 100
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

    // Bytes 3-4: Speed
    appendUInt16(packet, speed);

    // Bytes 5-6: Bearing
    appendUInt16(packet, bearing);

    // Bytes 7-8: CRC over bytes 0-6
    const quint16 crc = calculateCrc16(packet);
    appendUInt16(packet, crc);

    // Byte 9: Footer
    packet.append(static_cast<char>(0x55));

    return packet;
}

// Packet B payload:
// Bits 31-20: temperature multiplied by 10 (12 bits)
// Bits 19-12: equipment status (8 bits)
// Bits 11-0: battery voltage multiplied by 10 (12 bits)
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

    // Bytes 3-6: Packed 32-bit payload
    appendUInt32(packet, payload);

    // Bytes 7-8: CRC over bytes 0-6
    const quint16 crc = calculateCrc16(packet);
    appendUInt16(packet, crc);

    // Byte 9: Footer
    packet.append(static_cast<char>(0x55));

    return packet;
}
