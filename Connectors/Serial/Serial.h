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

#ifndef SERIAL_H
#define SERIAL_H

#include <QSerialPort>
#include "../../EmbeddedDebugger/Medium/Medium.h"
#include "Settingsdialog.h"

class ApplicationLayerBase;
class PresentationLayerBase;
class TransportLayerBase;

/**
 * @brief The Serial class will send all messages over a serial port.
 */
class Serial : public Medium
{
    Q_OBJECT
public:
    Serial(QObject* parent = nullptr);
    virtual ~Serial();

public slots:
    void connect() override;
    void disconnect() override;
    void showSettings() override;

private:
    void connectLayers();
    void destroyProtocolLayers();

private:
    QSerialPort m_serialPort; /**< QSerialPort instance */
    SettingsDialog m_settingsDialog; /**< Dialog for serial port settings */
};

#endif // SERIAL_H
