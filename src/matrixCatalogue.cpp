#include "global.h"

void MatrixCatalogue::insertMatrix(Matrix* matrix)
{
    logger.log("MatrixCatalogue::~insertMatrix");
    this->matrices[matrix->matrixName] = matrix;
}
Matrix* MatrixCatalogue::getMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::getMatrix");
    Matrix *matrix = this->matrices[matrixName];
    return matrix;
}
bool MatrixCatalogue::isMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::isMatrix");
    if (this->matrices.count(matrixName))
        return true;
    return false;
}
//
// void TableCatalogue::print()
// {
//     logger.log("TableCatalogue::print");
//     cout << "\nRELATIONS" << endl;
//
//     int rowCount = 0;
//     for (auto rel : this->tables)
//     {
//         cout << rel.first << endl;
//         rowCount++;
//     }
//     printRowCount(rowCount);
// }
