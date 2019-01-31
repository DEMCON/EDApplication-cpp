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

#include <utility>

#include "Register.h"
#include "Medium/CPU/Cpu.h"
#include <QDebug>

Register::Register(uint id, const QString& name, Register::ReadWrite readWrite, Register::VariableType variableType, Register::Source source, uint derefDepth, uint offset, Cpu& cpu) :
    m_id(id),
    m_name(name),
    m_readWrite(readWrite),
    m_variableType(variableType),
    m_source(source),
    m_derefDepth(derefDepth),
    m_offset(offset),
    m_cpu(cpu)
{}

/**
 * @brief Returns the Variable Size of this Register
 * @return Variable size of this Register
 */
int Register::getVariableTypeSize() const
{
     return m_cpu.getVariableTypeSize(m_variableType);
}

/**
 * @brief Configure this register as debugchannel
 * @param newChannelMode ChannelMode the register must be set to.
 */
void Register::configDebugChannel(Register::ChannelMode newChannelMode)
{
    m_channelMode = newChannelMode;
    emit configDebugChannel(*this);
}

/**
 * @brief Set the value of this register
 * If the type of the value is not the same as the current value, it will try to convert the value.
 * @param value New value to set this register to.
 */
void Register::setValue(const QVariant &value)
{
    if(m_registerValue.type() != value.type())
    {
        if (value.canConvert(static_cast<int>(m_registerValue.type())))
        {
            m_registerValue = const_cast<QVariant&>(value).convert(static_cast<int>(m_registerValue.type()));
            emit writeRegister(*this);
        }
    }
    else
    {
        m_registerValue = value;
        emit writeRegister(*this);
    }


}

/**
 * @brief Get a ReadWrite enum from a string value
 * @param enumString String you want to convert to a Enum
 * @return Enum value of the string
 */
Register::ReadWrite Register::ReadWritefromString(const QString& enumString)
{
    if(enumString == "Read") { return Register::ReadWrite::Read;}
    if(enumString == "Write"){ return Register::ReadWrite::Write;}

    qWarning() << "Unknown Read Write From String requested: " << enumString;
    return Register::ReadWrite::Unknown;

}

/**
 * @brief Get a Source enum from a string value
 * @param enumString String you want to convert to a Enum
 * @return Enum value of the string
 */
Register::Source Register::SourcefromString(const QString& enumString)
{
    if(enumString == "HandWrittenOffset") { return Register::Source::HandWrittenOffset;}
    if(enumString == "HandWrittenIndex")  { return Register::Source::HandWrittenIndex;}
    if(enumString == "SimulinkCApiOffset"){ return Register::Source::SimulinkCApiOffset;}
    if(enumString == "SimulinkCApiIndex") { return Register::Source::HandWrittenIndex;}
    if(enumString == "AbsoluteAddress")   { return Register::Source::AbsoluteAddress;}

    qWarning() << "Unknown Source from String requested: " << enumString;
    return Register::Source::Unknown;

}

/**
 * @brief Get a VariableType enum from a string value
 * @param enumString String you want to convert to a Enum
 * @return Enum value of the string
 */
Register::VariableType Register::variableTypeFromString(const QString& enumString)
{
    if (enumString == "pointer"){ return Register::VariableType::Pointer;}
    if(enumString == "bool"){ return Register::VariableType::Bool;}
    if(enumString == "int8_t"){ return Register::VariableType::Char;}
    if(enumString == "uint8_t"){ return Register::VariableType::Char;}
    if(enumString == "int16_t"){ return Register::VariableType::Short;}
    if(enumString == "uint16_t"){ return Register::VariableType::Short;}
    if(enumString == "int32_t"){ return Register::VariableType::Int;}
    if(enumString == "uint32_t"){ return Register::VariableType::Int;}
    if(enumString == "int64_t"){ return Register::VariableType::Long;}
    if(enumString == "uint64_t"){ return Register::VariableType::Long;}
    if(enumString == "float"){ return Register::VariableType::Float;}
    if(enumString == "double"){ return Register::VariableType::Double;}
    if(enumString == "long double"){ return Register::VariableType::Double;}


    qWarning() << "Unknown Variabletype from String requested: " << enumString;
    return Register::VariableType::Unknown;

}

/**
 * @brief Get a String from a variableType enum
 * @param variableType you want to convert to a string
 * @return String value of the enum
 */
QString Register::variableTypeToString(const Register::VariableType &variableType)
{
    switch(variableType)
    {
        case Register::VariableType::Pointer: return "Pointer";
        case Register::VariableType::Bool: return "Bool";
        case Register::VariableType::Char: return "Char";
        case Register::VariableType::Short: return "Short";
        case Register::VariableType::Int: return "Int";
        case Register::VariableType::Long: return "Long";
        case Register::VariableType::Float: return "Float";
        case Register::VariableType::Double: return "Double";
        case Register::VariableType::LongDouble: return "Long Double";
        default: return "Unknown";
    }
}


/**
 * @brief Slot called when a new register value is received.
 * @param newRegisterValue The new received value
 */
void Register::receivedNewRegisterValue(const QVariant& newRegisterValue)
{
    if (m_registerValue != newRegisterValue)
    {
        m_registerValue.setValue(QVariant::fromValue(newRegisterValue));
        emit registerDataChanged(*this);
    }
}

/**
 * @brief Slot called when a new register value is received including timestamp
 * @param newRegisterValue The new received value
 * @param timeStamp timestamp of the received value
 */
void Register::receivedNewRegisterValue(const QVariant& newRegisterValue, uint timeStamp)
{
    if (m_registerValue != newRegisterValue)
    {
        m_registerValue.setValue(newRegisterValue);
        m_lastRegisterValueTimestamp = timeStamp;
        emit registerDataChanged(*this);
    }
}
