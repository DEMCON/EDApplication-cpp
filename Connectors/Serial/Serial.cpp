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

#include "Serial.h"
#include <QMessageBox>
#include "../DebugProtocolV0/ApplicationLayerV0.h"
#include "../DebugProtocolV0/PresentationLayerV0.h"
#include "../DebugProtocolV0/TransportLayerV0.h"


Serial::Serial(QObject *parent) :
    Medium(parent),
    m_serialPort(this)
{
    m_availableProtocols.append("DebugProtocol V0");
}

Serial::~Serial()
{
    disconnect();
}

void Serial::connect()
{
    SettingsDialog::Settings p = m_settingsDialog.settings();

    if (p.name.isEmpty())
    {
        //Settings menu has not been opened. Load settings.
        m_settings.beginGroup("Serial");
        p.name = m_settings.value("Name").toString();
        p.baudRate = m_settings.value("BaudRate").toInt();
        p.dataBits = static_cast<QSerialPort::DataBits>(m_settings.value("DataBits").toInt());
        p.parity = static_cast<QSerialPort::Parity>(m_settings.value("Parity").toInt());
        p.stopBits = static_cast<QSerialPort::StopBits>(m_settings.value("StopBits").toInt());
        p.flowControl = static_cast<QSerialPort::FlowControl>(m_settings.value("FlowControl").toInt());
        m_settings.endGroup();

        if (p.name.isEmpty())
        {
            //Settings are not stored. Open Settings dialog
            showSettings();
        }
    }



        m_serialPort.setPortName(p.name);
        m_serialPort.setBaudRate(p.baudRate);
        m_serialPort.setDataBits(p.dataBits);
        m_serialPort.setParity(p.parity);
        m_serialPort.setStopBits(p.stopBits);
        m_serialPort.setFlowControl(p.flowControl);
        if (m_serialPort.open(QIODevice::ReadWrite))
        {
            switch(m_selectedProtocolVersion)
            {
                case 0:  createDebugProtocolV0Layers(); break;
            }
            connectLayers();
            static_cast<PresentationLayerV0*>(m_presentationLayer)->scanForCpu();
            setConnected(true);

            //Save Settings
            m_settings.beginGroup("Serial");
            m_settings.setValue("Name",p.name);
            m_settings.setValue("BaudRate",p.baudRate);
            m_settings.setValue("DataBits",p.dataBits);
            m_settings.setValue("Parity",p.parity);
            m_settings.setValue("StopBits",p.stopBits);
            m_settings.setValue("flowControl",p.flowControl);
            m_settings.endGroup();
        }
        else
        {
            emit errorOccured(m_serialPort.errorString());
        }

}

void Serial::disconnect()
{
    setConnected(false);
    m_serialPort.close();
    m_serialPort.reset();
    Medium::clear();
}

void Serial::showSettings()
{
    m_settingsDialog.show();
}

void Serial::setProtocolVersion(int availableProtocolVersionIndex)
{
    if(availableProtocolVersionIndex < m_availableProtocols.size())
    {
        m_selectedProtocolVersion = availableProtocolVersionIndex;
    }
}

void Serial::connectLayers()
{
    QObject::connect(&m_serialPort,&QSerialPort::readyRead, this, [&]()
    {
        m_transportLayer->receivedData(m_serialPort.readAll());
    });
    QObject::connect(m_transportLayer,&TransportLayerBase::write, this, [&](const QByteArray& message)
    {
        m_serialPort.write(message);
    });
}

void Serial::destroyProtocolLayers()
{
    QObject::disconnect(&m_serialPort, &QSerialPort::readyRead, this, nullptr); //Disconnect tcpSocket lambda
    Medium::destroyProtocolLayers();
}
