#include "global.h"
/**
 * @brief
 */
bool syntacticParseBULK_INSERT()
{
    logger.log("syntacticParseBULK_INSERT");
    if (tokenizedQuery.size() != 4 || tokenizedQuery[2] != "INTO")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = BULK_INSERT;
    parsedQuery.insertTableName = tokenizedQuery[3];
    parsedQuery.insertedTableName = tokenizedQuery[1];
    return true;
}

bool semanticParseBULK_INSERT()
{
    logger.log("semanticParseBULK_INSERT");

    if (! tableCatalogue.isTable(parsedQuery.insertTableName))
    {
        cout << "SEMANTIC ERROR: Insertion relation does not exist" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.insertedTableName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    
    Table* table = tableCatalogue.getTable(parsedQuery.insertTableName);
    int columnCount = table->columnCount;
    parsedQuery.loadRelationName = parsedQuery.insertedTableName;
    executeLOAD();
    Table* table1 = tableCatalogue.getTable(parsedQuery.insertedTableName);
    int columnCount1 = table1->columnCount;
    
    if(columnCount != columnCount1)
    {
	cout << "SEMANTIC ERROR: Number of columns in file is not Equal to number of columns in relation" << endl;
        return false;
    }
    return true;
}

void executeBULK_INSERT()
{
    logger.log("executeBULK_INSERT");
    Table* table = tableCatalogue.getTable(parsedQuery.insertTableName);
    Table* table1 = tableCatalogue.getTable(parsedQuery.insertedTableName);
    Cursor cursor = table1->getCursor();
    parsedQuery.insertValuesList = cursor.getNext();
    while (!parsedQuery.insertValuesList.empty()){
	executeINSERT();
	parsedQuery.insertValuesList = cursor.getNext();
    }
    tableCatalogue.deleteTable(parsedQuery.insertedTableName);
}
