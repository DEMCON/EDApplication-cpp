/*
Embedded Debugger PC Application which can be used to debug embedded systems at a high level.
Copyright (C) 2019 DEMCON advanced mechatronics B.V.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TRANSPORTLAYERBASE_H
#define TRANSPORTLAYERBASE_H

#include <QObject>
/**
 * @brief The TransportLayerBase class is the base class for the transport layer.
 * Each protocol version must be inherited from this class.
 */
class TransportLayerBase : public QObject
{
    Q_OBJECT
public:
    explicit TransportLayerBase(QObject* parent = nullptr) :
        QObject(parent){}

signals:

    /**
     * @brief Signal that is emitted when a new debug protocol message is received.
     * @param uCId cpuId where the message came from.
     * @param messageVector Vector containing the protocol message.
     */
    void receivedDebugProtocolMessage(uint8_t uCId, QVector<uint8_t> messageVector);

    /**
     * @brief Signal that is emitted when a new debug protocol command needs to be written.
     * @param message containing a debug protocol command.
     */
    void write(const QByteArray& message);

public slots:

    /**
     * @brief creates a valid debug protocol message from the cpuId and messageVector
     * @param uCId cpu where the message needs to be send to
     * @param messageVector vector containing the message that needs to be send.
     */
    virtual void sendDebugProtocolCommand(uint8_t uCId, QVector<uint8_t> messageVector) = 0;

    /**
     * @brief reads the incoming data for protocol messages.
     * When a protocol message is received, the receivedDebugProtocolCommand signal is emitted.
     * @param message containing the received data.
     */
    virtual void receivedData(const QByteArray message) = 0;

};


#endif // TRANSPORTLAYERBASE_H
