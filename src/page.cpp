#include "global.h"
/**
 * @brief Construct a new Page object. Never used as part of the code
 *
 */
Page::Page()
{
    this->pageName = "";
    this->tableName = "";
    this->pageIndex = -1;
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
    this->nextPointer = -1;
}

/**
 * @brief Construct a new Page:: Page object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tableName
 * @param pageIndex
 */
Page::Page(string tableName, int pageIndex)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);
    if(tableCatalogue.isTable(tableName))
    {
        Table table = *tableCatalogue.getTable(tableName);
        this->columnCount = table.columnCount;
        uint maxRowCount = table.maxRowsPerBlock;
        this->rowCount = table.rowsPerBlockCount[pageIndex];
        vector<int> row(columnCount, 0);
        this->rows.assign(maxRowCount, row);
    }
    else if(matrixCatalogue.isMatrix(tableName))
    {
        Matrix matrix = *matrixCatalogue.getMatrix(tableName);
        this->rowCount = matrix.dimsPerBlock[pageIndex].first;
        this->columnCount = matrix.dimsPerBlock[pageIndex].second;
        vector<int> row(columnCount, 0);
        this->rows.assign(rowCount, row);
    }

    ifstream fin(pageName, ios::in);
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    char marker;
    fin>>marker;
    if(marker=='>')
    {
        fin>>number;
        this->nextPointer=number;
    }
    else
    {
        this->nextPointer = -1;
    }
    fin.close();
}

/**
 * @brief Get row from page indexed by rowIndex
 *
 * @param rowIndex
 * @return vector<int>
 */
vector<int> Page::getRow(int rowIndex)
{
    logger.log("Page::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= this->rowCount)
        return result;
    return this->rows[rowIndex];
}

int Page::getNextPointer()
{
    return this->nextPointer;
}

Page::Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount, int nextPointer)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows[0].size();
    this->nextPointer = nextPointer;
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(pageIndex);
}

Page::Page(string tableName, int pageIndex, vector<int> row, int rowCount)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->row_toappend = row;
    this->rowCount = rowCount;
    this->columnCount = row.size();
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(pageIndex);
}
/**
 * @brief writes current page contents to file.
 *
 */
void Page::writePage()
{
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout<<">  ";
    fout<<this->nextPointer<<endl;
    fout.close();
}

void Page::appendPage()
{
    logger.log("Page::appendPage");
    ofstream fout(this->pageName, ios::app);
    bool first=true;
    for(auto it = this->row_toappend.begin();it!=row_toappend.end();it++)
    {
        if(!first)fout<<" ";
        if(first)first=false;
        fout << *it;
    }
    fout<< endl;
    fout.close();
}
