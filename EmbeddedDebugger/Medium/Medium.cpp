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

#include "Medium.h"

#include "../DebugProtocolV0/ApplicationLayerV0.h"
#include "../DebugProtocolV0/PresentationLayerV0.h"
#include "../DebugProtocolV0/TransportLayerV0.h"


Medium::Medium(QObject *parent) :
    QObject(parent)
{
    QObject::connect(&m_cpuListModel,&CpuListModel::newRegisterFound,this,[&](Register* newRegister)
    {
       if (m_applicationLayer != nullptr)
       {
           QObject::connect(newRegister,QOverload<Register&>::of(&Register::configDebugChannel),m_applicationLayer,&ApplicationLayerBase::configDebugChannel);
           QObject::connect(newRegister,&Register::writeRegister,m_applicationLayer,&ApplicationLayerBase::writeRegister);
           QObject::connect(newRegister,QOverload<Register&>::of(&Register::queryRegister),m_applicationLayer,&ApplicationLayerBase::queryRegister);
       }
       m_registerListModel.append(newRegister);
    });
}

void Medium::setConnected(bool isConnected)
{
    if(m_connected != isConnected)
    {
        m_connected = isConnected;
        emit connectedChanged();
    }
}

void Medium::createDebugProtocolV0Layers()
{
    destroyProtocolLayers();
    m_transportLayer = new TransportLayerV0(this);
    m_presentationLayer = new PresentationLayerV0(m_cpuListModel,m_registerListModel,this);
    m_applicationLayer = new ApplicationLayerV0(static_cast<PresentationLayerV0&>(*m_presentationLayer),this);
    QObject::connect(m_transportLayer,&TransportLayerBase::receivedDebugProtocolCommand,
                     m_presentationLayer,&PresentationLayerBase::receivedDebugProtocolCommand);
    QObject::connect(m_presentationLayer,&PresentationLayerBase::newDebugProtocolCommand,
                     m_transportLayer,&TransportLayerBase::sendDebugProtocolCommand);
    QObject::connect(m_presentationLayer,&PresentationLayerBase::newCpuFound,this, [&](Cpu* newCpu)
    {
        if (!m_cpuListModel.contains(newCpu->id()))
        {
            QObject::connect(newCpu,&Cpu::resetTime,m_applicationLayer,&ApplicationLayerBase::resetTime);
            QObject::connect(newCpu,QOverload<Cpu&>::of(&Cpu::setDecimation),m_applicationLayer,&ApplicationLayerBase::setDecimation);
            m_cpuListModel.append(newCpu);
        }
    });
}

void Medium::destroyProtocolLayers()
{
    if (m_applicationLayer != nullptr)
    {
        m_applicationLayer->deleteLater();
        m_applicationLayer = nullptr;
    }
    if (m_presentationLayer != nullptr)
    {
        m_presentationLayer->deleteLater();
        m_presentationLayer = nullptr;
    }
    if (m_transportLayer != nullptr)
    {
        m_transportLayer->deleteLater();
        m_transportLayer = nullptr;
    }
}

void Medium::clear()
{
    m_cpuListModel.clear();
    m_registerListModel.clear();
    destroyProtocolLayers();
}
