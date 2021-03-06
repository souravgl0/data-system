/**
 * @brief The Table class holds all information related to a loaded table. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a table object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use assignment statements (SELECT, PROJECT,
 * JOIN, SORT, CROSS and DISTINCT).
 *
 */
class Matrix
{
//     vector<unordered_set<int>> distinctValuesInColumns;
//
public:
    string sourceFileName = "";
    string matrixName = "";
    // vector<string> columns;
    // vector<uint> distinctValuesPerColumnCount;
    uint matrixDim = 0;
    uint smallDim = 0;
    uint bigDim = 0;
//     long long int rowCount = 0;
//     uint blockCount = 0;
//     uint maxRowsPerBlock = 0;
    vector<pair<int,int>> dimsPerBlock;
//     bool indexed = false;
//     string indexedColumn = "";
//     IndexingStrategy indexingStrategy = NOTHING;
//
    bool extractMetadata(string firstLine);
    bool blockify();
//     void updateStatistics(vector<int> row);
//     Table();
    Matrix(string tableName);
//     Table(string tableName, vector<string> columns);
    bool load();
    void transpose();
//     bool isColumn(string columnName);
//     void renameColumn(string fromColumnName, string toColumnName);
//     void print();
    void makePermanent();
//     bool isPermanent();
//     void getNextPage(Cursor *cursor);
//     Cursor getCursor();
//     int getColumnIndex(string columnName);
//     void unload();
//
//     /**
//  * @brief Static function that takes a vector of valued and prints them out in a
//  * comma seperated format.
//  *
//  * @tparam T current usaages include int and string
//  * @param row
//  */
// template <typename T>
// void writeRow(vector<T> row, ostream &fout)
// {
//     logger.log("Table::printRow");
//     for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
//     {
//         if (columnCounter != 0)
//             fout << ", ";
//         fout << row[columnCounter];
//     }
//     fout << endl;
// }
//
// /**
//  * @brief Static function that takes a vector of valued and prints them out in a
//  * comma seperated format.
//  *
//  * @tparam T current usaages include int and string
//  * @param row
//  */
// template <typename T>
// void writeRow(vector<T> row)
// {
//     logger.log("Table::printRow");
//     ofstream fout(this->sourceFileName, ios::app);
//     this->writeRow(row, fout);
//     fout.close();
// }
};
