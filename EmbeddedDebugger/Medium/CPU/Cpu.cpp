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

#include "Cpu.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

Cpu::Cpu(uint8_t id,const QString& name,const QString& serialNumber,const QString& protocolVersion,
         const QString& applicationVersion, QObject *parent) :
    QObject(parent),
    m_id(id),
    m_name(name),
    m_serialNumber(serialNumber),
    m_protocolVersion(protocolVersion),
    m_applicationVersion(applicationVersion)
{}

Cpu::~Cpu()
{

}

/**
 * @brief Set the variable type size
 * @param variableType where the size must be added to.
 * @param size size of the variableType
 */
void Cpu::setVariableTypeSize(const Register::VariableType &variableType, int size)
{
    m_variableTypeSizes.append(qMakePair(variableType,size));
}

/**
 * @brief Get the size of a VariableType
 * @param variableType Type of the size you want to know.
 * @return Size of the variableType
 */
int Cpu::getVariableTypeSize(const Register::VariableType& variableType)
{
    int returnValue = 0;
    for(auto variable : qAsConst(m_variableTypeSizes))
    {
        if(variable.first == variableType)
        {
            returnValue = variable.second;
            break;
        }
    }
    return returnValue;
}

/**
 * @brief Increase the invalid message counter
 */
void Cpu::increaseInvalidMessageCounter()
{
    increaseMessageCounter();
    m_invalidMessageCounter++;
}

/**
 * @brief Calculate the next available debug Channel
 * @return next available debug channel. If no debug channel is available, returns -1
 */
int Cpu::nextDebugChannel()
{
    if(m_debugChannels.size() < m_maxDebugChannels)
    {
        return m_debugChannels.size();
    }
    return -1;
}

/**
 * @brief Set the decimation of this Cpu
 * @param newDecimation the decimation to be set to the Cpu.
 */
void Cpu::setDecimation(int newDecimation)
{
    m_decimation = newDecimation;
    emit setDecimation(*this);
}

/**
 * @brief Load register configuration from json file.
 * @return true if loaded, false if not loaded.
 */
bool Cpu::loadConfiguration()
{
    //Load file from location.
    //TODO: Add UI part to load configuration from other location.
    QString fileLocation(QDir::currentPath() + "/Registers/" + m_name + "/" + m_applicationVersion+ ".json");
    QFile loadFile(fileLocation);
    //Check if file could be openend.
    if (!loadFile.open(QIODevice::ReadOnly))
    {
        qWarning() << "Could not open register List at location: " << fileLocation.toStdString().c_str();
        return false;
    }

    //Read registerData.
    QByteArray registerData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(registerData));
    QJsonObject registerObject = loadDoc.object();
    //Transpose Registers to JsonArray.
    QJsonArray registerAray = registerObject["Registers"].toArray();
    for (auto RegisterRef : registerAray)
    {
        QJsonObject Reg = RegisterRef.toObject();
        //Create a new Register from the Reg object.
        Register* newRegister = new Register(static_cast<uint>(Reg["id"].toInt()),
                Reg["name"].toString(),
                Register::ReadWritefromString(Reg["ReadWrite"].toString()),
                Register::variableTypeFromString(Reg["Type"].toString()),
                Register::SourcefromString(Reg["Source"].toString()),
                static_cast<uint>(Reg["DerefDepth"].toInt()),
                static_cast<uint>(Reg["Offset"].toInt()),
                *this);

        emit newRegisterFound(newRegister);
    }
    return true;
}

/**
 * @brief Received decimation
 * @param decimation that is received.
 */
void Cpu::receivedDecimation(int decimation)
{
    m_decimation = decimation;
}


