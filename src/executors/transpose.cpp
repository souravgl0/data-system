#include "global.h"
/**
 * @brief
 * SYNTAX: LOAD relation_name
 */
bool syntacticParseTRANSPOSE()
{
    logger.log("syntacticParseTRANSPOSE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = TRANSPOSE;
    parsedQuery.loadRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseTRANSPOSE()
{
    logger.log("semanticParseTRANSPOSE");

    if(! matrixCatalogue.isMatrix(parsedQuery.loadRelationName))
    {
        cout << "Matrix Not Loaded" << endl;
        return false;
    }

    return true;
}

void executeTRANSPOSE()
{
    logger.log("executeTRANSPOSE");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.loadRelationName);
    matrix->transpose();
    return;
}
