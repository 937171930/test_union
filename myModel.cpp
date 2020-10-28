#include "myModel.h"
#include <QDebug>

MyModel::MyModel()
{
}

QVariant MyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::TextAlignmentRole)
    {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    else if (role == Qt::DisplayRole)
    {
        return QStandardItemModel::data(index, role);
    }
    else if(role == Qt::BackgroundColorRole )
    {
        if(index.row()%2 == 1)
            return QColor(180,180,180);
        else
            return QVariant();
    }
    else if(role == Qt::BackgroundRole)
    {
        return QColor(255,255,255);
    }

    return QVariant();
}
