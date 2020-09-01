#include "global.h"

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName
 */
Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded
 * @return false if an error occurred
 */
bool Matrix::load()
{
    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractMetadata(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file.
 *
 * @param line
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Matrix::extractMetadata(string firstLine)
{
    logger.log("Matrix::extractMetadata");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    uint dim=0;
    while (getline(s, word, ','))
    {
        dim++;
    }
    this->matrixDim = dim;
    this->smallDim = (uint)sqrt((BLOCK_SIZE * 1000) / (32));
    this->bigDim = (uint)(this->matrixDim+this->smallDim-1)/this->smallDim;

    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size.
 *
 * @return true if successfully blockified
 * @return false otherwise
//  */
bool Matrix::blockify()
{
    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->smallDim, 0);
    int pagePrefix = 0;
    int srowInd=0;
    while (getline(fin, line))
    {
        stringstream s(line);
        int scolInd=0;
        int pageInd = pagePrefix;
        for (int i = 0; i < this->matrixDim; i++)
        {
            if (!getline(s, word, ','))
                return false;
            row[scolInd] = stoi(word);

            scolInd++;
            if(scolInd == this->smallDim)
            {
                bufferManager.appendPage(this->matrixName, pageInd, row,srowInd+1);

                scolInd=0;
                pageInd++;
            }
        }
        if(scolInd)
        {
            bufferManager.appendPage(this->matrixName, pageInd, vector<int>(row.begin(),row.begin()+scolInd),srowInd+1);
        }
        srowInd++;
        if(srowInd == this->smallDim)
        {
            pagePrefix+=this->bigDim;
            srowInd = 0;
        }
    }

    // cout<<bufferManager.getPage(this->matrixName,1)->rowCount<<endl;
    return true;
}
//
// /**
//  * @brief Given a row of values, this function will update the statistics it
//  * stores i.e. it updates the number of rows that are present in the column and
//  * the number of distinct values present in each column. These statistics are to
//  * be used during optimisation.
//  *
//  * @param row
//  */
// void Table::updateStatistics(vector<int> row)
// {
//     this->rowCount++;
//     for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
//     {
//         if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
//         {
//             this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
//             this->distinctValuesPerColumnCount[columnCounter]++;
//         }
//     }
// }
//
// /**
//  * @brief Checks if the given column is present in this table.
//  *
//  * @param columnName
//  * @return true
//  * @return false
//  */
// bool Table::isColumn(string columnName)
// {
//     logger.log("Table::isColumn");
//     for (auto col : this->columns)
//     {
//         if (col == columnName)
//         {
//             return true;
//         }
//     }
//     return false;
// }
//
// /**
//  * @brief Renames the column indicated by fromColumnName to toColumnName. It is
//  * assumed that checks such as the existence of fromColumnName and the non prior
//  * existence of toColumnName are done.
//  *
//  * @param fromColumnName
//  * @param toColumnName
//  */
// void Table::renameColumn(string fromColumnName, string toColumnName)
// {
//     logger.log("Table::renameColumn");
//     for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
//     {
//         if (columns[columnCounter] == fromColumnName)
//         {
//             columns[columnCounter] = toColumnName;
//             break;
//         }
//     }
//     return;
// }
//
// /**
//  * @brief Function prints the first few rows of the table. If the table contains
//  * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
//  * the rows are printed.
//  *
//  */
// void Table::print()
// {
//     logger.log("Table::print");
//     uint count = min((long long)PRINT_COUNT, this->rowCount);
//
//     //print headings
//     this->writeRow(this->columns, cout);
//
//     Cursor cursor(this->tableName, 0);
//     vector<int> row;
//     for (int rowCounter = 0; rowCounter < count; rowCounter++)
//     {
//         row = cursor.getNext();
//         this->writeRow(row, cout);
//     }
//     printRowCount(this->rowCount);
// }
//
//
//
// /**
//  * @brief This function returns one row of the table using the cursor object. It
//  * returns an empty row is all rows have been read.
//  *
//  * @param cursor
//  * @return vector<int>
//  */
// void Table::getNextPage(Cursor *cursor)
// {
//     logger.log("Table::getNext");
//
//         if (cursor->pageIndex < this->blockCount - 1)
//         {
//             cursor->nextPage(cursor->pageIndex+1);
//         }
// }
//
//
//
// /**
//  * @brief called when EXPORT command is invoked to move source file to "data"
//  * folder.
//  *
//  */
// void Table::makePermanent()
// {
//     logger.log("Table::makePermanent");
//     if(!this->isPermanent())
//         bufferManager.deleteFile(this->sourceFileName);
//     string newSourceFile = "../data/" + this->tableName + ".csv";
//     ofstream fout(newSourceFile, ios::out);
//
//     //print headings
//     this->writeRow(this->columns, fout);
//
//     Cursor cursor(this->tableName, 0);
//     vector<int> row;
//     for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
//     {
//         row = cursor.getNext();
//         this->writeRow(row, fout);
//     }
//     fout.close();
// }
//
// /**
//  * @brief Function to check if table is already exported
//  *
//  * @return true if exported
//  * @return false otherwise
//  */
// bool Table::isPermanent()
// {
//     logger.log("Table::isPermanent");
//     if (this->sourceFileName == "../data/" + this->tableName + ".csv")
//     return true;
//     return false;
// }
//
// /**
//  * @brief The unload function removes the table from the database by deleting
//  * all temporary files created as part of this table
//  *
//  */
// void Table::unload(){
//     logger.log("Table::~unload");
//     for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
//         bufferManager.deleteFile(this->tableName, pageCounter);
//     if (!isPermanent())
//         bufferManager.deleteFile(this->sourceFileName);
// }
//
// /**
//  * @brief Function that returns a cursor that reads rows from this table
//  *
//  * @return Cursor
//  */
// Cursor Table::getCursor()
// {
//     logger.log("Table::getCursor");
//     Cursor cursor(this->tableName, 0);
//     return cursor;
// }
// /**
//  * @brief Function that returns the index of column indicated by columnName
//  *
//  * @param columnName
//  * @return int
//  */
// int Table::getColumnIndex(string columnName)
// {
//     logger.log("Table::getColumnIndex");
//     for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
//     {
//         if (this->columns[columnCounter] == columnName)
//             return columnCounter;
//     }
// }
