#include "logic.h"

QString& getVariableByColumn(int column, ManagerRow& row)
{
    if (column == 0)
        return row.Name;
    else if (column == 1)
        return row.Login;
    else if (column == 3)
        return row.Pass;
    else if (column == 5)
        return row.Comment;
    throw std::out_of_range("index out of range");
}
