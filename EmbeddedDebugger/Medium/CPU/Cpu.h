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

#ifndef CPUNODE_H
#define CPUNODE_H

#include <QObject>
#include <QVector>
#include "Medium/Register/Register.h"

/**
 * @brief The Cpu class holds all the information about a cpu/node.
 */
class Cpu : public QObject
{
    Q_OBJECT

public:
    explicit Cpu(uint8_t id,const QString& name,const QString& serialNumber,const QString& protocolVersion,
                    const QString& applicationVersion, QObject* parent = nullptr);
    virtual ~Cpu();

    //Getters
    uint8_t id() const {return m_id;} /**< @brief Returns the id of the Cpu.*/
    QString name() const {return m_name;} /**< @brief Returns the name of the Cpu.*/
    QString serialNumber() const {return m_serialNumber;} /**< @brief Returns the serial number of the Cpu.*/
    QString protocolVersion() const {return m_protocolVersion;} /**< @brief Returns the protocol version of the Cpu.*/
    QString applicationVersion() const {return m_applicationVersion;} /**< @brief Returns the application version of the Cpu.*/
    int decimation() const {return m_decimation;} /**< @brief Returns the decimation of the Cpu.*/
    int messageCounter() const {return m_messageCounter;} /**< @brief Returns the message counter of the Cpu.*/
    int invalidMessageCounter() const {return m_invalidMessageCounter;} /**< @brief Returns the invalid message counter of the Cpu.*/

    void setVariableTypeSize(const Register::VariableType &variableType, int size);
    int getVariableTypeSize(const Register::VariableType& variableType);
    void increaseMessageCounter() {m_messageCounter++;} /**< Increase the message counter. */
    void increaseInvalidMessageCounter();
    void increaseNbrOfActiveDebugChannels() {m_activeDebugChannels++;} /**< Increase the number of active debug channels.*/
    void decreaseNbrOfActiveDebugChannels() {m_activeDebugChannels--;} /**< Decrease the number of active debug channels.*/
    int maxDebugChannels() const {return m_maxDebugChannels;} /**< @brief Returns the maximum debug channels */
    int nextDebugChannel();
    QVector<Register*>& debugChannels() {return m_debugChannels;} /**< @brief Returns a QVector containing the Registers that are active as debug channel.*/

signals:
    /**
     * @brief Reset the time of the Cpu.
     * @param cpu where the time should be reset.
     */
    void resetTime(Cpu& cpu);

    /**
     * @brief Request the decimation from the cpu.
     * @param cpu where the decimation should be readed from.
     */
    void getDecimation(Cpu& cpu);

    /**
     * @brief Set the decimation of the cpu.
     * @param cpu where the decimation must be set.
     */
    void setDecimation(Cpu& cpu);

    /**
     * @brief Signal that the decimation is changed.
     */
    void decimationChanged();

    /**
     * @brief Signal that a new register is found.
     * @param newRegister Register that is found.
     */
    void newRegisterFound(Register* newRegister);

public slots:
    void setDecimation(int newDecimation);
    bool loadConfiguration();
    void receivedDecimation(int decimation);

private:
    uint8_t m_id = 0; /**< Id of the Cpu.*/
    QString m_name; /**< Name of the Cpu.*/
    QString m_serialNumber; /**< Serial number of the Cpu.*/
    QString m_protocolVersion; /**< Protocol version of the Cpu.*/
    QString m_applicationVersion; /**< Application version of the Cpu.*/
    int m_activeDebugChannels = 0;  /**< Number of active debug channels.*/
    const int m_maxDebugChannels = 16;  /**< Maximum number of debug channels.*/
    int m_decimation = 0;  /**< Decimation of the Cpu.*/
    int m_messageCounter= 0;  /**< Message counter of the Cpu.*/
    int m_invalidMessageCounter = 0;  /**< Invalid message counter of the Cpu.*/
    QVector<Register*> m_debugChannels;  /**< Vector of registers that are used as debug channels.*/
    QVector<QPair<Register::VariableType,int>> m_variableTypeSizes;  /**< QVector of QPairs containing the sizes of different VariableTypes of the Cpu.*/
};

#endif // CPUNODE_H
