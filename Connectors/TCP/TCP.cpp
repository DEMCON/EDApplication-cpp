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

#include "TCP.h"
#include <QDebug>

#include "../DebugProtocolV0/ApplicationLayerV0.h"
#include "../DebugProtocolV0/PresentationLayerV0.h"
#include "../DebugProtocolV0/TransportLayerV0.h"


TCP::TCP(QObject* parent) :
    Medium(parent),
    m_tcpSocket(this)
{
    m_availableProtocols.append("DebugProtocol V0");

    QObject::connect(&m_tcpSocket,&QTcpSocket::connected, this, [&]()
    {
        if(m_presentationLayer != nullptr)
        {
            static_cast<PresentationLayerV0*>(m_presentationLayer)->scanForCpu();
            setConnected(true);
        }
    });
    QObject::connect(&m_tcpSocket,&QTcpSocket::disconnected, this, [&](){setConnected(false);});

    QObject::connect(&m_tcpSocket,QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, [&]()
    {
        emit errorOccured(m_tcpSocket.errorString());
        qDebug() << m_tcpSocket.errorString();
    });
}

TCP::~TCP()
{
    disconnect();
}

void TCP::connectLayers()
{
   QObject::connect(&m_tcpSocket,&QTcpSocket::readyRead, this, [&]()
    {
        m_transportLayer->receivedData(m_tcpSocket.readAll());
    });
    QObject::connect(m_transportLayer,&TransportLayerBase::write, this, [&](const QByteArray& message)
    {
        m_tcpSocket.write(message);
    });
}

void TCP::destroyProtocolLayers()
{
      QObject::disconnect(&m_tcpSocket, &QTcpSocket::readyRead, this, nullptr); //Disconnect tcpSocket lambda
      Medium::destroyProtocolLayers();
}


void TCP::connect()
{
    m_settings.beginGroup("TCP");

    QString hostname = m_settings.value("IPAddress","").toString();
    bool portConverted;
    uint16_t port = static_cast<uint16_t>(m_settings.value("IPPort",0).toInt(&portConverted));

    m_settings.endGroup();
    if (hostname.isEmpty() ||
        !portConverted ||
        port == 0)
    {
       showSettings();
    }
    else
    {
        switch(m_selectedProtocolVersion)
        {
            case 0:  createDebugProtocolV0Layers(); break;
        }

        connectLayers();
        m_tcpSocket.connectToHost(hostname,port);
    }
}

void TCP::disconnect()
{
    m_tcpSocket.disconnectFromHost();
    m_tcpSocket.reset();
    Medium::clear();
}

void TCP::showSettings()
{
    m_tcpSettingsDialog.show();
}

bool TCP::setHostAddress(const QString &ipAddress, int ipPort)
{
    m_hostPort = ipPort;
    return m_hostAddress.setAddress(ipAddress);
}

void TCP::setProtocolVersion(int availableProtocolVersionIndex)
{
    if(availableProtocolVersionIndex < m_availableProtocols.size())
    {
        m_selectedProtocolVersion = availableProtocolVersionIndex;
    }
}
