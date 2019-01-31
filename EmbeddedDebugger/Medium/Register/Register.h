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

#ifndef REGISTER_H
#define REGISTER_H

#include <QVariant>
#include <QObject>
class Cpu;

/**
 * @brief The Register class hold all the information from a register.
 */
class Register : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Used to check if a register is a read or write register.
     */
    enum class ReadWrite{
        Unknown,
        Read,
        Write
    };

    /**
     * @brief Channel mode the register has in a debug channel.
     */
    enum class ChannelMode{
        Off = 0,
        OnChange = 0x1,
        LowSpeed = 0x2,
        Once = 0x03
    };

    /**
     * @brief Source of the register
     */
    enum class Source{
        HandWrittenOffset = 0,
        HandWrittenIndex = 0x10,
        SimulinkCApiOffset = 0x40,
        SimulinkCApiIndex = 0x50,
        AbsoluteAddress = 0x70,
        Unknown,
    };

    /**
     * @brief Possible variable types of the register
     */
    enum class VariableType
    {
        MemoryAlignment = 0x0, // memory alignment (given in size_n; typically 1 or 4; example: memory alignment = 4  addresses are a multiple of 4)
        Pointer = 0x1,
        Bool = 0x2,
        Char = 0x3,
        Short = 0x4,
        Int = 0x5,
        Long = 0x6,
        Float = 0x7,
        Double = 0x8,
        LongDouble = 0x9,
        TimeStamp = 0xA,        // time-stamp units in μs (uses 4 bytes for size_n!)
        Unknown,

    };

    Register(uint id,const QString& name, Register::ReadWrite readWrite, Register::VariableType variableType, Register::Source source, uint derefDepth, uint offset, Cpu& cpu);

    uint id() const {return m_id;} /**< @brief Returns the id of the Register.*/
    QString name() const {return m_name;} /**< @brief Returns the name of the Register.*/
    Register::ReadWrite readWrite() const {return m_readWrite;} /**< @brief Returns the ReadWrite status of the Register.*/
    Register::ChannelMode channelMode() const {return m_channelMode;} /**< @brief Returns the channelmode of the Register.*/
    Register::Source source() const {return m_source;} /**< @brief Returns the source of the Register.*/
    Register::VariableType variableType() const {return m_variableType;} /**< @brief Returns the VariableType of the Register.*/
    int getVariableTypeSize() const;
    uint derefDepth() const {return m_derefDepth;} /**< @brief Returns the derefDepth of the Register.*/
    uint32_t offset() const {return m_offset;} /**< @brief Returns the offset of the Register.*/
    uint timeStampUnits() const {return m_timeStampUnits;} /**< @brief Returns the timestampUnits of the Register.*/
    QVariant value() const {return m_registerValue;} /**< @brief Returns the value of the Register.*/
    uint timeStamp() const {return m_lastRegisterValueTimestamp;} /**< @brief Returns the timestamp of the Register.*/
    Cpu& cpu() const {return m_cpu;} /**< @brief Returns the cpu of the Register.*/
    void configDebugChannel(ChannelMode newChannelMode);
    void setValue(const QVariant &value);
    void queryRegister() {emit queryRegister(*this);} /**< @brief Query this Register */

    static Register::ReadWrite ReadWritefromString(const QString& enumString);
    static Register::Source SourcefromString(const QString&  enumString);
    static Register::VariableType variableTypeFromString(const QString&  enumString);
    static QString variableTypeToString(const Register::VariableType&  variableType);

public slots:
    void receivedNewRegisterValue(const QVariant& newRegisterValue);
    void receivedNewRegisterValue(const QVariant& newRegisterValue, uint timeStamp);

signals:
    void configDebugChannel(Register& Register);
    void writeRegister(Register& Register);
    void queryRegister(Register& Register);
    void registerDataChanged(Register& Register);

private:
    uint m_id;  /**< @brief The id of the Register.*/
    QString m_name;  /**< @brief The name of the Register.*/
    Register::ReadWrite m_readWrite;  /**< @brief current ReadWrite of the Register.*/
    Register::VariableType m_variableType;  /**< @brief variableType of the Register.*/
    Register::ChannelMode m_channelMode = Register::ChannelMode::Off;  /**< @brief The ChannelMode of the Register.*/
    Register::Source m_source;  /**< @brief The soure of the Register.*/
    uint m_derefDepth = 0; /**< @brief The derefDepth of the Register.*/
    uint32_t m_offset = 0;  /**< @brief The offset of the Register.*/
    uint m_timeStampUnits = 0;  /**< @brief The timeStampUnits of the Register.*/
    QVariant m_registerValue;  /**< @brief The value of the Register.*/
    uint m_lastRegisterValueTimestamp = 0;  /**< @brief The last timestamp of the received value of the Register.*/
    Cpu& m_cpu; /**< @brief Reference to the Cpu of this Register.*/
};

#endif // REGISTER_H
