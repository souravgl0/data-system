#include "global.h"

Cursor::Cursor(string tableName, int pageIndex)
{
    logger.log("Cursor::Cursor");
    this->page = bufferManager.getPage(tableName, pageIndex);
    this->pagePointer = 0;
    this->tableName = tableName;
    this->pageIndex = pageIndex;
}

/**
 * @brief This function reads the next row from the page. The index of the
 * current row read from the page is indicated by the pagePointer(points to row
 * in page the cursor is pointing to).
 *
 * @return vector<int>
 */
vector<int> Cursor::getNext()
{
    logger.log("Cursor::geNext");
    vector<int> result = this->page.getRow(this->pagePointer);
    this->pagePointer++;
    if(result.empty()){
        tableCatalogue.getTable(this->tableName)->getNextPage(this);
        if(!this->pagePointer){
            result = this->page.getRow(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}

vector<vector<int>> Cursor::getWholePage()
{
    logger.log("Cursor::getWholePage");

    vector<int> row(this->page.columnCount, 0);
    vector<vector<int>> rows;
    rows.assign(this->page.rowCount, row);

    for(int i = 0; i<this->page.rowCount ; i++)
    {
        vector<int> result = this->page.getRow(this->pagePointer);
        this->pagePointer++;
        for(int j=0;j<this->page.columnCount; j++)
        {
            rows[i][j] = result[j];
        }
    }

    return rows;
}
/**
 * @brief Function that loads Page indicated by pageIndex. Now the cursor starts
 * reading from the new page.
 *
 * @param pageIndex
 */
void Cursor::nextPage(int pageIndex)
{
    logger.log("Cursor::nextPage");
    this->page = bufferManager.getPage(this->tableName, pageIndex);
    this->pageIndex = pageIndex;
    this->pagePointer = 0;
}

void Cursor::nextLinkedPage()
{
    logger.log("Cursor::nextLinkedPage");
    int nextPage = (this->page).getNextPointer();
    if(nextPage!=-1)
    {
        this->page = bufferManager.getPage(this->tableName, nextPage);
    }
    this->pageIndex = nextPage;
    this->pagePointer = 0;
}

vector<int> Cursor::getNextLinked()
{
    logger.log("Cursor::geNextLinked");
    vector<int> result = this->page.getRow(this->pagePointer);
    this->pagePointer++;
    if(result.empty()){
        this->nextLinkedPage();
        if(this->pageIndex != -1){
            result = this->page.getRow(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}
