#include "global.h"
/**
 * @brief
 */
bool syntacticParseINSERT()
{
    logger.log("syntacticParseINSERT");
    if (tokenizedQuery.size() < 5 || tokenizedQuery[3] != "VALUES" || tokenizedQuery[1] != "INTO")
    {
        cout << "SYNTAC ERROR " << endl;
        return false;
    }
    parsedQuery.queryType = INSERT;
    parsedQuery.insertTableName = tokenizedQuery[2];
    int tokens = tokenizedQuery.size();

    try{
        for(int i=4;i<tokens;i++)
        {
            parsedQuery.insertValuesList.push_back(stoi(tokenizedQuery[i]));
        }
    }
    catch(std::invalid_argument& e){
        cout << "SYNTAC ERROR " << endl;
        return false;
    }
    catch(const std::exception& e){
        cout << "SYNTAC ERROR UNDERFINED" << endl;

        return false;
    }
    return true;
}

bool semanticParseINSERT()
{
    logger.log("semnticParseINSERT");

    if (! tableCatalogue.isTable(parsedQuery.insertTableName))
    {
        cout << "SEMANTIC ERROR: Insertion relation does not exist" << endl;
        return false;
    }

    Table* table = tableCatalogue.getTable(parsedQuery.insertTableName);
    int columnCount = table->columnCount;

    if(columnCount != parsedQuery.insertValuesList.size())
    {
        cout << "SEMANTIC ERROR: Number of Values Passed is not Equal to number of columns in relation" << endl;
        return false;
    }
    return true;
}

void executeINSERT()
{
    logger.log("executeINSERT");
    Table* table = tableCatalogue.getTable(parsedQuery.insertTableName);
    if(table->indexed)
    {
        table->insertRowUsingIndex(parsedQuery.insertValuesList);
        return;
    }
    Page page = bufferManager.getPage(table->tableName,table->blockCount-1);
    if(page.rowCount < table->maxRowsPerBlock)
    {
        Cursor cursor(table->tableName,table->blockCount-1);
        vector<vector<int>> rows = cursor.getWholePage();
        rows.push_back(parsedQuery.insertValuesList);

        Page pageNew(table->tableName,cursor.pageIndex,rows,page.rowCount + 1,page.nextPointer);
        pageNew.writePage();
        bufferManager.removeFromPool(table->tableName,cursor.pageIndex);
        table->rowsPerBlockCount[cursor.pageIndex]=page.rowCount + 1;
        table->rowCount++;
    }
    else
    {
        vector<vector<int>> rows = {parsedQuery.insertValuesList};
        Page pageNew(table->tableName,table->blockCount , rows ,1,-1);
        pageNew.writePage();
        table->rowsPerBlockCount.push_back(1);
        table->rowCount++;
        table->blockCount++;
    }
}
