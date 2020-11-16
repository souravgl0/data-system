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

}
