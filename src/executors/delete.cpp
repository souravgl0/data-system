#include "global.h"
/**
 * @brief
 */
bool syntacticParseDELETE()
{
    logger.log("syntacticParseDELETE");
    if (tokenizedQuery.size() < 5 || tokenizedQuery[3] != "VALUES" || tokenizedQuery[1] != "FROM")
    {
        cout << "SYNTAC ERROR " << endl;
        return false;
    }
    parsedQuery.queryType = DELETE;
    parsedQuery.deleteTableName = tokenizedQuery[2];
    int tokens = tokenizedQuery.size();

    try{
        for(int i=4;i<tokens;i++)
        {
            parsedQuery.deleteValuesList.push_back(stoi(tokenizedQuery[i]));
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

bool semanticParseDELETE()
{
    logger.log("semnticParseDELETE");

    if (! tableCatalogue.isTable(parsedQuery.deleteTableName))
    {
        cout << "SEMANTIC ERROR: Deletion relation does not exist" << endl;
        return false;
    }

    Table* table = tableCatalogue.getTable(parsedQuery.deleteTableName);
    int columnCount = table->columnCount;

    if(columnCount != parsedQuery.deleteValuesList.size())
    {
        cout << "SEMANTIC ERROR: Number of Values Passed is not Equal to number of columns in relation" << endl;
        return false;
    }
    return true;
}

void executeDELETE()
{
    logger.log("executeDELETE");
    Table* table = tableCatalogue.getTable(parsedQuery.deleteTableName);
    if(table->indexed)
    {
        table->deleteRowUsingIndex(parsedQuery.deleteValuesList);
        return;
    }
    Cursor cursor(table->tableName,0);
    for(int curPage=0;curPage<table->blockCount;curPage++)
    {
        cursor.nextPage(curPage);
        vector<vector<int>> rows = cursor.getWholePage();
        auto it = rows.begin();
        bool deleteFlag = false;
        int numRowsDeleted = 0;
        while(it != rows.end())
        {
            if(*it == parsedQuery.deleteValuesList)
            {
                it = rows.erase(it);
                deleteFlag = true;
                numRowsDeleted++;
            }
            else it++;
        }
        if(deleteFlag)
        {
            Page page = bufferManager.getPage(table->tableName,curPage);
            Page newPage(table->tableName,curPage,rows,rows.size(),page.nextPointer);
            newPage.writePage();
            bufferManager.removeFromPool(table->tableName,curPage);
            table->rowsPerBlockCount[curPage]=rows.size();
            table->rowCount -= numRowsDeleted;
        }
    }
}
