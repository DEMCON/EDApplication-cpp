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

#ifndef COMMON_H
#define COMMON_H

#include <QByteArray>
#include <QVector>
#include <QVariant>
#include <QDataStream>
#include <QDebug>

/**
 * @brief Common functions for reading and writing data
 */


/**
 * @brief Create QVector<uint8_t> from a QByteArray
 * @param data QBytearray containing the data.
 * @param size of the QVector that should be returned.
 * @return QVector<uint8_t> containing the data.
 */
static QVector<uint8_t> fromByteArray(QByteArray data, uint8_t size)
{
    QVector<uint8_t> vector(size);
    for(int i = 0; i < vector.size(); i++)
    {
        vector.replace(i,static_cast<uint8_t>(data[i]));
    }
    return vector;
}

/**
 * @brief template toByteArray
 * Convert data to QByteArray.
 * Only supports dataTypes that are supported by QByteArray
 */
template<typename data_type>
QByteArray toByteArray(data_type data)
{
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << data;
    return bytes;
};

/**
 * @brief template toValue
 * Converts a QVector<uint8_t> to a return_type.
 * Only supports dataTypes that are supported by QByteArray
 */
template<typename return_type>
return_type toValue(QVector<uint8_t> data)
{
    QByteArray arrayValue = QByteArray::fromRawData(reinterpret_cast<const char*>(data.data()),data.size());
    QDataStream stream(&arrayValue, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    return_type value;
    stream >> value;
    return value;
}

/**
 * @brief append 32bit value to QVector<uint8_t>
 * @param appendToVector vector that the 32 bits value needs to be added
 * @param valueToAppend uint32_t value that needs to be added to the QVector
 */
inline void append32BitValue(QVector<uint8_t>& appendToVector,const uint32_t valueToAppend){
    appendToVector.append(static_cast<uint8_t>(valueToAppend));
    appendToVector.append(static_cast<uint8_t>(valueToAppend >>  8));
    appendToVector.append(static_cast<uint8_t>(valueToAppend >> 16));
    appendToVector.append(static_cast<uint8_t>(valueToAppend >> 24));
}

/**
 * @brief Convert QVaraint to QVector<uint8_t>
 * @param data QVaraint that needs to be converted.
 * @param size The size of the QVector that needs to be returned.
 * @return QVector<uint8_t> that contains the converted value.
 */
inline QVector<uint8_t> toQVector(const QVariant& data, const uint8_t size)
{
    switch(data.type())
    {
        case QVariant::Bool:
            {
                return fromByteArray(toByteArray(data.toBool()),size);

            }
        default: return QVector<uint8_t>();
    }

}


#endif // COMMON_H
