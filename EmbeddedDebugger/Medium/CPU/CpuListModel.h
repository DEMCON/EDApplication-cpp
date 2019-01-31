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

#ifndef CPUNODELISTMODEL_H
#define CPUNODELISTMODEL_H

#include <QAbstractListModel>
class Cpu;
class Register;

/**
 * @brief The CpuListModel class holds all the cpu`s that are found.
 */
class CpuListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit CpuListModel(QObject *parent = nullptr);
     ~CpuListModel() override;

    //Basic funtionality:
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void insert(int index, Cpu* cpuNode);
    void append(Cpu* cpuNode);
    void clear();
    bool contains(uint8_t nodeId);
    Cpu* getCpuNodeById(uint8_t cpuNodeID);

signals:

    /**
     * @brief Signal that is emitted when a new register is found.
     * @param newRegister the register that is found.
     */
    void newRegisterFound(Register* newRegister);

private:
    QVector<Cpu*> m_cpuNodes; /**< QVector containing all the cpu`s. */
};

#endif // CPUNODELISTMODEL_H
