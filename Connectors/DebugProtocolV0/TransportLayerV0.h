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

#ifndef TRANSPORTLAYERV0_H
#define TRANSPORTLAYERV0_H

#include "../BaseInterface/TransportLayerBase.h"
#include "../BaseInterface/Common.h"

/**
 * @brief The TransportLayerV0 class implements the functions needed to send the debug protocol.
 * It will add the STX,ETX,CRC etc to make it a valid debug protocol message.
 */
class TransportLayerV0 : public TransportLayerBase
{
    Q_OBJECT
public:
    explicit TransportLayerV0(QObject *parent = nullptr);
    virtual ~TransportLayerV0();

public slots:
    /**
    * @copydoc TransportLayerBase::sendDebugProtocolCommand()
    */
    void sendDebugProtocolCommand(uint8_t uCId, QVector<uint8_t> messageVector) override;

    /**
    * @copydoc TransportLayerBase::receivedData()
    */
    void receivedData(const QByteArray message) override;

private:
    /**
     * @brief calculates the messageId for the protocol message that needs to be send.
     * MsgId 0 will never be returned, because of the debug protocol states:
     * Msg-ID = 0 is not ACK by the slaves.
     * This msg-ID can be used by the PC when sending a cmd to all slaves at once
     * @return MessageID that will be used in the next protocol command that is send.
     */
    uint8_t msgId();

    /**
     * @brief Calculates the crc from the messageVector
     * @param messageVector calculates the crc from this messageVector
     * @return Crc of the messageVector.
     */
    uint8_t calculateCRC(const QVector<uint8_t>& messageVector);

    /**
     * @brief Adds escape characters to the protocol message.
     * @param messageVector The messageVector where the escape chars must be added to.
     */
    void addEscapeCharacters(QVector<uint8_t>& messageVector);

    /**
     * @brief Replaces escape characters from the protocol message
     * @param messageVector The messageVector where the escape chars must be removed from.
     */
    void replaceEscapeCharacters(QVector<uint8_t>& messageVector);

private:
    uint8_t m_msgId = 0; /**< current messageID */
    QByteArray m_dataBuffer; /**< ByteArray containing received data that is used as buffer to store unprocessed protocol commands */
};

#endif // TRANSPORTLAYERV0_H
