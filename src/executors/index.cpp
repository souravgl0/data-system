#include "global.h"
/**
 * @brief
 * SYNTAX: INDEX ON column_name FROM relation_name USING indexing_strategy
 * indexing_strategy: ASC | DESC | NOTHING
 */
bool syntacticParseINDEX()
{
    logger.log("syntacticParseINDEX");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[1] != "ON" || tokenizedQuery[3] != "FROM" || tokenizedQuery[5] != "USING")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = INDEX;
    parsedQuery.indexColumnName = tokenizedQuery[2];
    parsedQuery.indexRelationName = tokenizedQuery[4];
    string indexingStrategy = tokenizedQuery[6];
    if (indexingStrategy == "BTREE")
    {
        parsedQuery.indexingStrategy = BTREE;
        string fanouts = tokenizedQuery[8];
        regex numeric("[-]?[0-9]+");
        if (!regex_match(fanouts, numeric)){
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        parsedQuery.btreeFanout = stoi(fanouts);

    }
    else if (indexingStrategy == "HASH")
    {
        parsedQuery.indexingStrategy = HASH;
        string buckets = tokenizedQuery[8];
        regex numeric("[-]?[0-9]+");
        if (!regex_match(buckets, numeric)){
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        parsedQuery.hashBuckets = stoi(buckets);
    }
    else if (indexingStrategy == "NOTHING")
        parsedQuery.indexingStrategy = NOTHING;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseINDEX()
{
    logger.log("semanticParseINDEX");
    if (!tableCatalogue.isTable(parsedQuery.indexRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    if (!tableCatalogue.isColumnFromTable(parsedQuery.indexColumnName, parsedQuery.indexRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    Table* table = tableCatalogue.getTable(parsedQuery.indexRelationName);
    if(table->indexed){
        cout << "SEMANTIC ERROR: Table already indexed" << endl;
        return false;
    }
    return true;
}

void executeINDEX()
{
    logger.log("executeINDEX");
    Table* table = tableCatalogue.getTable(parsedQuery.indexRelationName);
    table->buildIndex(parsedQuery.indexingStrategy,parsedQuery.indexColumnName);
    return;
}
