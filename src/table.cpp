#include "global.h"

/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table()
{
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName
 */
Table::Table(string tableName)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName
 * @param columns
 */
Table::Table(string tableName, vector<string> columns)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (32 * columnCount));
    this->writeRow<string>(columns);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded
 * @return false if an error occurred
 */
bool Table::load()
{
    logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractColumnNames(line))
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
bool Table::extractColumnNames(string firstLine)
{
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))
            return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (32 * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size.
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify()
{
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    getline(fin, line);
    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock)
        {
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row
 */
void Table::updateStatistics(vector<int> row)
{
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName
 * @return true
 * @return false
 */
bool Table::isColumn(string columnName)
{
    logger.log("Table::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName
 * @param toColumnName
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
    logger.log("Table::print");
    uint count = min((long long)PRINT_COUNT, this->rowCount);

    //print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}



/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor
 * @return vector<int>
 */
void Table::getNextPage(Cursor *cursor)
{
    logger.log("Table::getNext");

        if (cursor->pageIndex < this->blockCount - 1)
        {
            cursor->nextPage(cursor->pageIndex+1);
        }
}



/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{
    logger.log("Table::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
    logger.log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
    return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload(){
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 *
 * @return Cursor
 */
Cursor Table::getCursor()
{
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 *
 * @param columnName
 * @return int
 */
int Table::getColumnIndex(string columnName)
{
    logger.log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
}

void Table::buildIndex(IndexingStrategy indexingStrategy, string indexColumnName)
{
    logger.log("Table::buildIndex");
    if(indexingStrategy == HASH){
        if(! this->buildHashIndex(indexColumnName)){
            cout<<"Error in building Index";
            return;
        }
    }
    if(indexingStrategy != NOTHING)this->indexed = true;
    this->indexingStrategy = indexingStrategy;
    this->indexedColumn = indexColumnName;
    cout<<"Index Built Successfully"<<endl;
}

bool Table::buildHashIndex(string indexColumnName)
{
    logger.log("Table::buildHashIndex");
    this->hashIndex  = new LinearHash(4);

    // Copy current pages to another temporary location
    // For building index, the records will be reordered
    // in new blocks then these temporary pages will be deleted.
    int tempPageIndex = 3*rowCount;

    this->rowsPerBlockCount.resize(this->blockCount+tempPageIndex);
    for (int pageIndex = 0; pageIndex < this->blockCount; pageIndex++)
    {
        bufferManager.renameFile(this->tableName, pageIndex, pageIndex + tempPageIndex);
        bufferManager.removeFromPool(this->tableName,pageIndex);
        this->rowsPerBlockCount[pageIndex + tempPageIndex] = this->rowsPerBlockCount[pageIndex];
        this->rowsPerBlockCount[pageIndex] = 0;
    }
    this->blockCount = tempPageIndex + this->blockCount;

    // Creating new blocks clustered according to the index attribute
    int columnIndex = this->getColumnIndex(indexColumnName);
    Cursor cursor(this->tableName, tempPageIndex);
    vector<int> row;
    int nextPageIndex = 0;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext();
        int key = row[columnIndex];

        int insertedPageIndex = hashIndex->insert(key,nextPageIndex);
        writeInLinkedPages(row,insertedPageIndex, nextPageIndex);
        if(insertedPageIndex == nextPageIndex)nextPageIndex++;
        this->distinctValuesInIndexedColumn.insert(key);
    }

    // Remove copied blocks
    // Also remove from Pool
    for (int pageIndex = 0; pageIndex < this->blockCount; pageIndex++)
    {
        bufferManager.removeFromPool(this->tableName,pageIndex + tempPageIndex);
        bufferManager.deleteFile(this->tableName,pageIndex + tempPageIndex);
    }
    this->rowsPerBlockCount.resize(nextPageIndex);
    this->blockCount = nextPageIndex;
    return true;
}

void Table::writeInLinkedPages(vector<int> row, int insertedPageIndex, int& nextPageIndex)
{
    logger.log("Table::writeInLinkedPages");
    if(insertedPageIndex != nextPageIndex)
    {
        Cursor cursor(this->tableName,insertedPageIndex);
        while(cursor.page.rowCount == this->maxRowsPerBlock)
        {
            cursor.nextLinkedPage();
        }
        vector<vector<int>>rows = cursor.getWholePage();
        int curRowCount = cursor.page.rowCount;
        rows.push_back(row);
        curRowCount++;
        int nextPagePointer = -1;
        if(curRowCount==this->maxRowsPerBlock)
        {
            nextPagePointer = nextPageIndex;
            Page page(this->tableName,nextPageIndex,vector<vector<int>>(),0,-1);
            page.writePage();
            nextPageIndex++;
        }
        Page page(this->tableName,cursor.pageIndex,rows,curRowCount,nextPagePointer);
        page.writePage();
        bufferManager.removeFromPool(this->tableName,cursor.pageIndex);
        this->rowsPerBlockCount[cursor.pageIndex]=curRowCount;
    }
    else
    {
        int curRowCount = 1;
        Page page(this->tableName,nextPageIndex,vector<vector<int>>({row}),1,-1);
        this->rowsPerBlockCount[nextPageIndex]=1;
        if(curRowCount == this->maxRowsPerBlock){
            page.nextPointer = nextPageIndex;

            Page page2(this->tableName,nextPageIndex,vector<vector<int>>(),0,-1);
            page2.writePage();

            nextPageIndex++;
        }
        page.writePage();
    }
}

void Table::executeSelectQuery()
{
    logger.log("Table::executeSelectQuery");
    int columnIndex = this->getColumnIndex(parsedQuery.selectionFirstColumnName);
    Table* resultantTable = new Table(parsedQuery.selectionResultRelationName, this->columns);

    set<int> querySet = this->getQuerySet(columnIndex);
    for(int resultantVal:querySet)
    {
        int pageIndex;
        if(this->indexingStrategy == HASH)
        {
            pageIndex = hashIndex->find(resultantVal);
        }
        else if (this->indexingStrategy == BTREE)
        {

        }
        if(pageIndex != -1)
        {
            vector<int> row;
            Cursor cursor(this->tableName,pageIndex);
            row = cursor.getNextLinked();
            while(!row.empty())
            {
                resultantTable->writeRow<int>(row);
                row = cursor.getNextLinked();
            }
        }
    }
    if(resultantTable->blockify())
        tableCatalogue.insertTable(resultantTable);
    else{
        cout<<"Empty Table"<<endl;
        delete resultantTable;
    }
}

set<int> Table::getQuerySet(int columnIndex)
{
    BinaryOperator queryOperator = parsedQuery.selectionBinaryOperator;
    int val = parsedQuery.selectionIntLiteral;
    set<int> res;
    set<int> valueSet = this->distinctValuesInIndexedColumn;
    if(queryOperator == EQUAL)
    {
        auto it = valueSet.find(val);
        if(it != valueSet.end())
        {
            res.insert(val);
            return res;
        }
    }
    else if(queryOperator == LEQ || queryOperator == LESS_THAN)
    {
        auto it = valueSet.upper_bound(val);
        it--;
        for(;it != valueSet.begin(); it--)
        {
            res.insert(*it);
        }
        res.insert(*it);
        if(queryOperator == LESS_THAN)res.erase(val);
        return res;
    }
    else if(queryOperator == GEQ || queryOperator == GREATER_THAN)
    {
        auto it = valueSet.lower_bound(val);
        for(;it != valueSet.end(); it++)
        {
            res.insert(*it);
        }
        if(queryOperator == GREATER_THAN)res.erase(val);
        return res;
    }
}

void Table::insertRowUsingIndex(vector<int> row)
{
    
}
