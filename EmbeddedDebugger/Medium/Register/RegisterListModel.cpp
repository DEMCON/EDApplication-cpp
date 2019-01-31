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

#include "RegisterListModel.h"
#include "Medium/CPU/Cpu.h"
#include <QDebug>

RegisterListModel::RegisterListModel(QObject* parent) :
    QAbstractTableModel(parent)
{
}

RegisterListModel::~RegisterListModel()
{
    clear();
}

/**
 * @brief Returns the number of rows.
 * @param parent parent of this object, Not used.
 * @return number of rows.
 */
int RegisterListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_registers.count();
}

/**
 * @brief Returns the number of columns.
 * @param parent parent of this object, Not used.
 * @return number of columns.
 */
int RegisterListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 6;
}

/**
 * @brief Returns the item flags for the given index.
 * @param index of the item you want the flags from
 * @return flags for the given index.
 */
Qt::ItemFlags RegisterListModel::flags(const QModelIndex &index) const
{
    if (index.column() == 3)
    {
        if (index.isValid() &&
                index.row() < m_registers.size() &&
                index.row() >= 0)
        {
            const auto &Register = m_registers.at(index.row());
            if( Register->readWrite() == Register::ReadWrite::Write)
            {
                return Qt::ItemIsEnabled | Qt::ItemIsEditable;
            }
        }
    }

    return Qt::ItemIsEnabled;
}

/**
 * @brief Returns the data stored under the given role for the item referred to by the index.
 * @param index of the data object
 * @param role of the data object
 * @return data stored under the given role for the item.
 */
QVariant RegisterListModel::data(const QModelIndex &index, int role) const
{
    QVariant returnValue;

    if (index.isValid() &&
            index.row() < m_registers.size() &&
            index.row() >= 0 &&
            (role == Qt::DisplayRole ||
             role == Qt::EditRole))
    {
        const auto &Register = m_registers.at(index.row());

        switch(index.column())
        {
            case 0:
                {
                    returnValue = Register->cpu().id();
                    break;
                }
            case 1:
                {
                    returnValue =  Register->name();
                    break;
                }
            case 2:
                {
                    returnValue =  Register::variableTypeToString(Register->variableType());
                    break;
                }
            case 3:
                {
                    if(!Register->value().isNull())
                    {
                        returnValue =  Register->value();
                    }
                    break;
                }
            case 4:
                {
                    returnValue = static_cast<uint8_t>(Register->channelMode());
                    break;
                }
            default: break;
        }
    }
    return returnValue;
}

/**
 * @brief Set data to a Register
 * @param index of the register
 * @param value new value that needs to be set
 * @param role role of the data object
 * @return True if value is set, false if value is not set.
 */
bool RegisterListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool returnValue = false;
    if (index.isValid() &&
            index.row() < m_registers.size() &&
            index.row() >= 0 &&
            (role == Qt::DisplayRole ||
             role == Qt::EditRole))
    {
        const auto &Register = m_registers.at(index.row());

        switch(index.column())
        {
            case 3:
                {
                    Register->setValue(value);
                    returnValue = true;
                    break;
                }
            case 4:
                {
                    //Channel Mode
                    Register->configDebugChannel(static_cast<Register::ChannelMode>(value.toInt()));
                    returnValue = true;
                    break;
                }
            case 5:
                {
                    //Refesh Value
                    Register->queryRegister();
                    returnValue = true;
                    break;
                }
            default:
                {
                    break;
                }
        }
    }
    return returnValue;
}

/**
 * @brief Returns the data for the given role and section in the header with the specified orientation.
 * @param section section of the header.
 * @param orientation orientation of the header.
 * @param role role of the header.
 * @return data for the given role and section.
 */
QVariant RegisterListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant returnValue;
    if (role == Qt::DisplayRole &&
            orientation == Qt::Horizontal)
    {
        switch (section) {
            case 0:
                {
                    returnValue = tr("Cpu ID");
                    break;
                }
            case 1:
                {
                    returnValue = tr("Name");
                    break;
                }
            case 2:
                {
                    returnValue = tr("Type");
                    break;
                }
            case 3:
                {
                    returnValue = tr("Value");
                    break;
                }
            case 4:
                {
                    returnValue = tr("Channel mode");
                    break;
                }
            case 5:
                {
                    returnValue = tr("Refresh");
                    break;
                }
            default: break;
        }
    }
    return returnValue;
}

/**
 * @brief insert data to the list model.
 * @param index where the data should be added.
 * @param registerNode Register that needs to be added to the list.
 */
void RegisterListModel::insert(int index, Register* registerNode)
{
    if(index < 0)
    {
        registerNode->deleteLater();
        return;
    }
    beginInsertRows(QModelIndex(), index, index);
    registerNode->setParent(this);
    connect(registerNode,&Register::registerDataChanged,this,&RegisterListModel::registerDataChanged);
    m_registers.insert(index,registerNode);
    endInsertRows();
}

/**
 * @brief append a cpu to the model list.
 * @param registerNode to append to the list.
 */
void RegisterListModel::append(Register* registerNode)
{
    insert(m_registers.count(),registerNode);
}

/**
 * @brief clear the list and remove all the Registers.
 */
void RegisterListModel::clear()
{
    beginResetModel();
    for (auto registerNode : qAsConst(m_registers))
    {
        registerNode->deleteLater();
    }
    m_registers.clear();
    endResetModel();
}

/**
 * @brief Get a Register.
 * @param uCId cpu id of the register.
 * @param offset offset of the register.
 * @param readWrite readwrite status of the register.
 * @return if register is found, a pointer to the register, else a nullptr will be returned.
 */
Register *RegisterListModel::getRegisterByCpuIdOffsetReadWrite(uint8_t uCId, uint32_t offset, Register::ReadWrite readWrite)
{
    Register* returnValue = nullptr;

    for (auto tempRegister : qAsConst(m_registers))
    {
        if (tempRegister->cpu().id() == uCId &&
                tempRegister->offset() == offset &&
                tempRegister->readWrite() == readWrite)
        {
            returnValue = tempRegister;
        }
    }
    return returnValue;
}

/**
 * @brief Slot called when a Register is changed.
 * This funcion will refresh the correct line in the QTableView.
 * @param Register where the data is changed from.
 */
void RegisterListModel::registerDataChanged(Register &Register)
{
    int row = m_registers.indexOf(&Register);
    QModelIndex startOfRow = this->index(row, 0);
    QModelIndex endOfRow   = this->index(row, columnCount(QModelIndex())-1);


    emit dataChanged(startOfRow,endOfRow, {Qt::DisplayRole});
}
