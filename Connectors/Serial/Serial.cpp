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
}

Serial::~Serial()
{
    disconnect();
}

/**
 * @brief Connect to a serial port and find all cpu`s
 * If the user as never used the serialport, the SettingsDialog will be openend.
 */
void Serial::connect()
{
    const SettingsDialog::Settings p = m_settingsDialog.settings();

    if (p.name.isEmpty())
    {
        //Settings are not stored. Open Settings dialog
        showSettings();
    }
    else
    {
        m_serialPort.setPortName(p.name);
        m_serialPort.setBaudRate(p.baudRate);
        m_serialPort.setDataBits(p.dataBits);
        m_serialPort.setParity(p.parity);
        m_serialPort.setStopBits(p.stopBits);
        m_serialPort.setFlowControl(p.flowControl);
        if (m_serialPort.open(QIODevice::ReadWrite))
        {   
            createDebugProtocolV0Layers();
            connectLayers();
            setConnected(true);
            static_cast<PresentationLayerV0*>(m_presentationLayer)->scanForCpu();
        }
        else
        {
            emit errorOccured(m_serialPort.errorString());
        }
    }
}

/**
 * @brief Disconnect from the serial port and destory all layers.
 */
void Serial::disconnect()
{
    setConnected(false);
    destroyProtocolLayers();
    m_serialPort.close();
    m_serialPort.reset();
    Medium::clear();
}

/**
 * @brief Show settings dialog.
 */
void Serial::showSettings()
{
    m_settingsDialog.show();
}

/**
 * @brief Connect signals and slots for the layers.
 */
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

/**
 * @brief Destory signals and protocol layers.
 */
void Serial::destroyProtocolLayers()
{
    QObject::disconnect(&m_serialPort, &QSerialPort::readyRead, this, nullptr); //Disconnect tcpSocket lambda
    Medium::destroyProtocolLayers();
}
