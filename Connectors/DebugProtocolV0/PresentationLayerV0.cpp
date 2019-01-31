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

#include <QDebug>
#include <QVector>
#include "PresentationLayerV0.h"
#include "../DebugProtocolV0/DebugProtocolV0Enums.h"
#include "Medium/CPU/CpuListModel.h"
#include "Medium/CPU/Cpu.h"

PresentationLayerV0::PresentationLayerV0(CpuListModel& cpuListModel, RegisterListModel& registerListModel, QObject *parent) :
    PresentationLayerBase(cpuListModel,registerListModel, parent)
{}

PresentationLayerV0::~PresentationLayerV0()
= default;

void PresentationLayerV0::receivedDebugProtocolCommand(uint8_t uCID, QVector<uint8_t> protocolCommand)
{
    Cpu* cpu = m_cpuListModel.getCpuNodeById(uCID);
    if(cpu != nullptr)
    {
        cpu->increaseMessageCounter();
    }

    switch(protocolCommand.takeFirst())
    {
        case DebugProtocolV0Enums::ProtocolCommand::GetVersion:
            {
                receivedGetVersion(uCID, protocolCommand);
                break;
            }
        case DebugProtocolV0Enums::ProtocolCommand::GetInfo:
            {
                receivedGetInfo(uCID,protocolCommand);
                break;
            }
        case DebugProtocolV0Enums::ProtocolCommand::WriteRegister:
            {
                receivedWriteRegister(uCID,protocolCommand);
                break;
            }
        case DebugProtocolV0Enums::ProtocolCommand::QueryRegister:
            {
                receivedQueryRegister(uCID,protocolCommand);
                break;
            }
        case DebugProtocolV0Enums::ProtocolCommand::ReadChannelData:
            {
                receivedReadChannelData(uCID,protocolCommand);
                break;
            }

        default:
            {

                break;
            }
    }
}

void PresentationLayerV0::scanForCpu()
{
    QVector<uint8_t> debugProtocolMessage;
    debugProtocolMessage.append(DebugProtocolV0Enums::GetVersion);
    emit newDebugProtocolCommand(0xFF, debugProtocolMessage); //Send GetVersion to all cpu's
    qDebug() << "Send scanForCpu";
}

void PresentationLayerV0::queryRegister(const Register &registerToRead)
{
    QVector<uint8_t> newDebugProtocolMessage;
    newDebugProtocolMessage.append(DebugProtocolV0Enums::QueryRegister);
    append32BitValue(newDebugProtocolMessage, registerToRead.offset());
    newDebugProtocolMessage.append(calculateControlByte(registerToRead));
    newDebugProtocolMessage.append(registerToRead.getVariableTypeSize());
    emit newDebugProtocolCommand(registerToRead.cpu().id(),newDebugProtocolMessage);
}

void PresentationLayerV0::writeRegister(const Register &registerToWrite)
{
    QVector<uint8_t> newDebugProtocolMessage;
    newDebugProtocolMessage.append(DebugProtocolV0Enums::WriteRegister);
    append32BitValue(newDebugProtocolMessage, registerToWrite.offset());
    newDebugProtocolMessage.append(calculateControlByte(registerToWrite));
    newDebugProtocolMessage.append(registerToWrite.getVariableTypeSize());
    newDebugProtocolMessage.append(toQVector(registerToWrite.value(),registerToWrite.getVariableTypeSize()));
    emit newDebugProtocolCommand(registerToWrite.cpu().id(),newDebugProtocolMessage);
}

void PresentationLayerV0::resetTime(uint8_t uCId)
{
    QVector<uint8_t> newDebugProtocolMessage;
    newDebugProtocolMessage.append(DebugProtocolV0Enums::ResetTime);
    emit newDebugProtocolCommand(uCId,newDebugProtocolMessage);

}

void PresentationLayerV0::configDebugChannel(Register &registerToConfigDebugChannel)
{
    QVector<uint8_t> newDebugProtocolMessage;
    newDebugProtocolMessage.append(DebugProtocolV0Enums::ConfigChannel);
    int debugChannel = registerToConfigDebugChannel.cpu().debugChannels().indexOf(&registerToConfigDebugChannel);
    if (debugChannel >= 0)
    {
        //Debugchannel already exists. only need to change channelmode
        newDebugProtocolMessage.append(static_cast<uint8_t>(debugChannel));
        newDebugProtocolMessage.append(static_cast<uint8_t>(registerToConfigDebugChannel.channelMode()));
        emit newDebugProtocolCommand(registerToConfigDebugChannel.cpu().id(),newDebugProtocolMessage);

        if (registerToConfigDebugChannel.channelMode() == Register::ChannelMode::Off)
        {
            registerToConfigDebugChannel.cpu().debugChannels().removeOne(&registerToConfigDebugChannel);
        }
    }
    else
    {
        //Debugchannel does not exists.
        debugChannel = registerToConfigDebugChannel.cpu().nextDebugChannel();
        if(debugChannel >= 0)
        {
            registerToConfigDebugChannel.cpu().debugChannels().append(&registerToConfigDebugChannel);
            newDebugProtocolMessage.append(static_cast<uint8_t>(debugChannel));
            newDebugProtocolMessage.append(static_cast<uint8_t>(registerToConfigDebugChannel.channelMode()));
            append32BitValue(newDebugProtocolMessage, registerToConfigDebugChannel.offset());
            newDebugProtocolMessage.append(calculateControlByte(registerToConfigDebugChannel));
            newDebugProtocolMessage.append(registerToConfigDebugChannel.getVariableTypeSize());
            emit newDebugProtocolCommand(registerToConfigDebugChannel.cpu().id(),newDebugProtocolMessage);
        }
    }
}

void PresentationLayerV0::getDecimation(uint8_t uCId)
{
    QVector<uint8_t> debugProtocolMessage;
    debugProtocolMessage.append(DebugProtocolV0Enums::Decimation);
    emit newDebugProtocolCommand(uCId, debugProtocolMessage);
}

void PresentationLayerV0::setDecimation(uint8_t uCId,int newDecimation)
{
    QVector<uint8_t> debugProtocolMessage;
    debugProtocolMessage.append(DebugProtocolV0Enums::Decimation);
    debugProtocolMessage.append(toQVector(newDecimation,1));
    emit newDebugProtocolCommand(uCId, debugProtocolMessage);
}

void PresentationLayerV0::receivedGetVersion(uint8_t &uCId, const QVector<uint8_t> &commandData)
{
    //Check if message is large enough to be a Get version command.
    if(commandData.size() < 9)
    {
        qWarning() << "Message too short for version message";
        increaseInvalidMessageCounter(uCId);
    }
    else
    {
        QString protocolVersion;
        QString applicationVersion;
        QString name;
        QString serialNumber;

        //Get Protocol version from commandData.
        protocolVersion = QStringLiteral("%1.%2.%3.%4").arg(QString::number(commandData.value(0)),
                                                            QString::number(commandData.value(1)),
                                                            QString::number(commandData.value(2)),
                                                            QString::number(commandData.value(3)));

        //Get Application version from commandData.
        applicationVersion = QStringLiteral("%1.%2.%3.%4").arg(QString::number(commandData.value(4)),
                                                               QString::number(commandData.value(5)),
                                                               QString::number(commandData.value(6)),
                                                               QString::number(commandData.value(7)));

        //Get name from commandData
        uint8_t nameLength = commandData.value(8);
        for(int i = 9; i < 9+nameLength ; i++)
        {
            name.append(commandData.value(i));
        }

        //Get serial from commandData.
        uint8_t serialLength = commandData.value(9 + nameLength);
        for(int i = 10 + nameLength; i < 10 + nameLength + serialLength; i++)
        {
            serialNumber.append(commandData.value(i));
        }

        //Create new cpu from extracted data.
        auto* cpu = new Cpu(uCId,name,serialNumber,protocolVersion,applicationVersion);
        //Increase the valid messagecounter from the cpu.
        cpu->increaseMessageCounter();
        //Emit that we have found a new Cpu.
        emit newCpuFound(cpu);
        //Disable All Cpu debugChannels
        disableAllConfigChannels(uCId,static_cast<uint8_t>(cpu->maxDebugChannels()));
        //Request info from the Cpu.
        sendGetInfo(cpu->id());
    }
}

void PresentationLayerV0::receivedWriteRegister(uint8_t &uCId, const QVector<uint8_t> &commandData)
{
    Q_UNUSED(uCId);
    if(commandData.size() == 1)
    {
        switch(commandData[0])
        {
            case 0x00: break; //ok, value is written
            case 0x01: qWarning() << "Invalid (offset) address at writing value"; break;
            case 0x02: qWarning() << "error dereferencing (null-pointer appeared at some dereference)"; break;
            default: qWarning() << "received unknown writeRegister result valuez"; break;
        }
    }
    else
    {
        increaseInvalidMessageCounter(uCId);
    }
}

void PresentationLayerV0::receivedQueryRegister(uint8_t &uCId, const QVector<uint8_t> &commandData)
{    
    //Check if messageSize is smaller than 7. If this is so, the message is not valid
    if(commandData.size() < 7)
    {
        qWarning() << "Received query register commmand from uC: " << uCId << " is invalid";
        //Get Cpu to increase invalidMessageCounter
        increaseInvalidMessageCounter(uCId);
    }
    else
    {
        auto offset = toValue<quint32>(commandData.mid(0,4));
        uint8_t ctrl = commandData[4];
        uint8_t size = commandData[5];
        //Get register by using the offset, control byte, and size.
        Register* reg = m_registerListModel.getRegisterByCpuIdOffsetReadWrite(uCId,offset,getReadWriteFromControlByte(ctrl));
        if (reg != nullptr)
        {
            //If register is found. convert the value to the corrent variable type.
            switch (reg->variableType())
            {
                case Register::VariableType::Bool:
                    {
                        reg->receivedNewRegisterValue(toValue<bool>(commandData.mid(6,size)));
                        break;
                    }
                case Register::VariableType::Char:
                    {
                        reg->receivedNewRegisterValue(toValue<uint8_t>(commandData.mid(6,size)));
                        break;
                    }
                case Register::VariableType::Short:
                    {
                        reg->receivedNewRegisterValue(toValue<int16_t>(commandData.mid(6,size)));
                        break;
                    }
                case Register::VariableType::Int:
                    {
                        reg->receivedNewRegisterValue(toValue<int32_t>(commandData.mid(6,size)));
                        break;
                    }
                case Register::VariableType::Long:
                    {
                        reg->receivedNewRegisterValue(toValue<int64_t>(commandData.mid(6,size)));
                        break;
                    }
                case Register::VariableType::Float:
                    {
                        reg->receivedNewRegisterValue(toValue<float>(commandData.mid(6,size)));
                        break;
                    }
                case Register::VariableType::Double:
                    {
                        reg->receivedNewRegisterValue(toValue<double>(commandData.mid(6,size)));
                        break;
                    }
                case Register::VariableType::LongDouble:
                    {
                        reg->receivedNewRegisterValue(toValue<double>(commandData.mid(6,size)));
                        break;
                    }
                default:
                    {
                        qWarning() << "Received unknown variableType" << static_cast<int>(reg->variableType());
                        break;
                    }
            }
        }
        else
        {
            qWarning() << "Received Query Register from unknown register offset, ctrl byte or cpu id;";
            qDebug() << "Offset: " << offset;
            qDebug() << "Control Byte" << ctrl;
            qDebug() << "uCId: " << uCId;
        }
    }
}

void PresentationLayerV0::receivedDecimation(uint8_t uCId, const QVector<uint8_t> &commandData)
{
    Q_UNUSED(uCId);
    Q_UNUSED(commandData);
    //TODO: Implement this function
}

void PresentationLayerV0::receivedReadChannelData(uint8_t uCId, QVector<uint8_t> &commandData)
{
    //Find cpu with the uCId;
    Cpu* cpu = m_cpuListModel.getCpuNodeById(uCId);
    if(cpu != nullptr)
    {
        //If cpu is found, check the if the message is large enough to contain the read channel data.
        if(commandData.size() < 5)
        {
            qWarning() << "Received read channel datacommmand from uC: " << uCId << " is invalid";
            increaseInvalidMessageCounter(uCId,cpu);
        }

        auto time = static_cast<uint>(((commandData[2] << 16) | (commandData[1] << 8) | commandData[0]));
        auto mask = toValue<uint8_t>(commandData.mid(3,2));

        //Remove first 5 characters. these containt the time, and masking, so not needed anymore.
        commandData.remove(0,5);

        //Loop over all the active debug channels.
        //From max to min, because highest channel is send first.
        for (int i = cpu->debugChannels().size(); i >= 0 ; i--)
        {
            //Check if mask contains the current debug channel.
            if ((mask >> i & 1) == 1)
            {
                //Get register from the current debug channel.
                Register* reg = cpu->debugChannels().value(i);
                if(reg != nullptr)
                {
                    //Convert the value of the debug channel to the correct variable Type.
                    switch (reg->variableType())
                    {
                        case Register::VariableType::Bool:
                            {
                                reg->receivedNewRegisterValue(toValue<bool>(commandData.mid(0,reg->getVariableTypeSize())),time);
                                break;
                            }
                        case Register::VariableType::Char:
                            {
                                reg->receivedNewRegisterValue(toValue<uint8_t>(commandData.mid(0,reg->getVariableTypeSize())),time);
                                break;
                            }
                        case Register::VariableType::Short:
                            {
                                reg->receivedNewRegisterValue(toValue<int16_t>(commandData.mid(0,reg->getVariableTypeSize())),time);
                                break;
                            }
                        case Register::VariableType::Int:
                            {
                                reg->receivedNewRegisterValue(toValue<int32_t>(commandData.mid(0,reg->getVariableTypeSize())),time);
                                break;
                            }
                        case Register::VariableType::Long:
                            {
                                reg->receivedNewRegisterValue(toValue<int64_t>(commandData.mid(0,reg->getVariableTypeSize())),time);
                                break;
                            }
                        case Register::VariableType::Float:
                            {
                                reg->receivedNewRegisterValue(toValue<float>(commandData.mid(0,reg->getVariableTypeSize())),time);
                                break;
                            }
                        case Register::VariableType::Double:
                            {
                                reg->receivedNewRegisterValue(toValue<double>(commandData.mid(0,reg->getVariableTypeSize())),time);
                                break;
                            }
                        case Register::VariableType::LongDouble:
                            {
                                reg->receivedNewRegisterValue(toValue<double>(commandData.mid(0,reg->getVariableTypeSize())),time);
                                break;
                            }
                        default: break;
                    }

                    //Remove the data from the vector. current data is not needed anymore.
                    commandData.remove(commandData.size() - reg->getVariableTypeSize() ,reg->getVariableTypeSize());
                }
            }
        }
    }
}

void PresentationLayerV0::receivedDebugString(uint8_t uCId, const QVector<uint8_t> &commandData)
{
    Q_UNUSED(uCId);
    Q_UNUSED(commandData);
    //TODO: implement this function.
}

void PresentationLayerV0::receivedGetInfo(uint8_t uCId,QVector<uint8_t>& commandData)
{
    //Check if Cpu exists in list
    Cpu* cpu = m_cpuListModel.getCpuNodeById(uCId);

    if(cpu != nullptr)
    {
        // Check if message is too small to be a get Info command
        if (commandData.size() < 2)
        {
            qWarning() << "Invalid GetInfo received";
            increaseInvalidMessageCounter(uCId,cpu);
        }
        else
        {
            QVector<uint8_t> record;
            //Loop over the commandData to extract variable type sizes.
            for (QVector<uint8_t>::iterator it=commandData.begin(); it != commandData.end(); ++it)
            {
                //If current iterator is a Record Seperator or iterator is at the end of the commandData
                if (*it == static_cast<char>(DebugProtocolV0Enums::ProtocolChar::RS) || it == commandData.end())
                {
                    //Check if record[0] is a timeStamp
                    if (record[0] == static_cast<uint8_t>(Register::VariableType::TimeStamp))
                    {
                        //Timestamp is 4 byte
                        cpu->setVariableTypeSize(Register::VariableType::TimeStamp,int(record[4] << 24 | record[3] << 16 | record[2] << 8 | record[1]));
                    }
                    else
                    {
                        //Everything else is 1 byte.
                        cpu->setVariableTypeSize(static_cast<Register::VariableType>(record[0]),record[1]);
                    }
                    //clear current record.
                    record.clear();
                }
                else
                {
                    record.append(*it);
                }
            }
        }
    }
}

void PresentationLayerV0::sendGetVersion(uint8_t uCId)
{
    QVector<uint8_t> debugProtocolMessage;
    debugProtocolMessage.append(DebugProtocolV0Enums::GetVersion);
    emit newDebugProtocolCommand(uCId, debugProtocolMessage);
}

void PresentationLayerV0::sendGetInfo(uint8_t uCId)
{
    QVector<uint8_t> debugProtocolMessage;
    debugProtocolMessage.append(DebugProtocolV0Enums::GetInfo);
    emit newDebugProtocolCommand(uCId, debugProtocolMessage);

}

void PresentationLayerV0::disableAllConfigChannels(uint8_t uCId, uint8_t nbrOfConfigChannels)
{
    QVector<uint8_t> newDebugProtocolMessage;
    for(int i = 0; i < nbrOfConfigChannels; i++)
    {
        newDebugProtocolMessage.append(DebugProtocolV0Enums::ConfigChannel);
        newDebugProtocolMessage.append(static_cast<uint8_t>(i));
        newDebugProtocolMessage.append(static_cast<uint8_t>(Register::ChannelMode::Off));
        emit newDebugProtocolCommand(uCId,newDebugProtocolMessage);
        newDebugProtocolMessage.clear();
    }
}

/**
 * @brief Increase the invalid message counter of the Cpu
 * @param uCId Id from the Cpu where the invalid message counter must be increased.
 * @param cpu If the code already found the cpu, the function will not search for the Cpu.
 */
void PresentationLayerV0::increaseInvalidMessageCounter(uint8_t uCId, Cpu *cpu)
{
    Cpu* Cpu = cpu;
    if (Cpu == nullptr)
    {
        Cpu = m_cpuListModel.getCpuNodeById(uCId);
    }
    if (Cpu != nullptr)
    {
        Cpu->increaseInvalidMessageCounter();
    }
}

uint8_t PresentationLayerV0::calculateControlByte(const Register &Register)
{
    uint8_t control = 0;
    control |= Register.readWrite() == Register::ReadWrite::Write ? 0x80 : 0x00;
    control |= static_cast<uint8_t>(Register.source());
    control |= (Register.derefDepth() & 0x0F);
    return control;
}

Register::ReadWrite PresentationLayerV0::getReadWriteFromControlByte(uint8_t ctrlByte)
{
    if ((ctrlByte >> 7) == 1)
    {
        return Register::ReadWrite::Write;
    }
    return Register::ReadWrite::Read;
}
