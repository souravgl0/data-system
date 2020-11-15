#ifndef PAGE_H
#define PAGE_H

#include"logger.h"
/**
 * @brief The Page object is the main memory representation of a physical page
 * (equivalent to a block). The page class and the page.h header file are at the
 * bottom of the dependency tree when compiling files.
 *<p>
 * Do NOT modify the Page class. If you find that modifications
 * are necessary, you may do so by posting the change you want to make on Moodle
 * or Teams with justification and gaining approval from the TAs.
 *</p>
 */

class Page{

    string tableName;
    string pageIndex;
    vector<vector<int>> rows;
    vector<int> row_toappend;

    public:
        int nextPointer;
        int columnCount=0;
        int rowCount=0;

    string pageName = "";
    Page();
    Page(string tableName, int pageIndex);
    Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount, int nextPointer = -1);
    Page(string tableName, int pageIndex, vector<int> row, int rowCount);
    vector<int> getRow(int rowIndex);
    int getNextPointer();
    void writePage();
    void appendPage();
};

#endif
