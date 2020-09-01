#include "global.h"

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName
 */
Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded
 * @return false if an error occurred
 */
bool Matrix::load()
{
    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractMetadata(line))
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
bool Matrix::extractMetadata(string firstLine)
{
    logger.log("Matrix::extractMetadata");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    uint dim=0;
    while (getline(s, word, ','))
    {
        dim++;
        for(auto cc:word){
            if(!isdigit(cc)){
                cout<<"Error: Matrix not Valid, Non-digit character in first row"<<endl;
                return false;
            }
        }
    }
    this->matrixDim = dim;
    this->smallDim = (uint)sqrt((BLOCK_SIZE * 1000) / (32));
    this->bigDim = (uint)(this->matrixDim+this->smallDim-1)/this->smallDim;

    this->dimsPerBlock.assign(this->bigDim*this->bigDim, make_pair(0,0));
    for(int ind = 0;ind<this->bigDim*this->bigDim; ind++)
    {
        bufferManager.deleteFile(this->matrixName,ind);
    }
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size.
 *
 * @return true if successfully blockified
 * @return false otherwise
//  */
bool Matrix::blockify()
{
    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->smallDim, 0);
    int pagePrefix = 0;
    int srowInd=0;
    while (getline(fin, line))
    {
        stringstream s(line);
        int scolInd=0;
        int pageInd = pagePrefix;
        for (int i = 0; i < this->matrixDim; i++)
        {
            if (!getline(s, word, ','))
                return false;
            row[scolInd] = stoi(word);

            scolInd++;
            if(scolInd == this->smallDim)
            {
                bufferManager.appendPage(this->matrixName, pageInd, row,srowInd+1);
                this->dimsPerBlock[pageInd] = make_pair(srowInd+1,scolInd);
                scolInd=0;
                pageInd++;
            }
        }
        if(scolInd)
        {
            bufferManager.appendPage(this->matrixName, pageInd, vector<int>(row.begin(),row.begin()+scolInd),srowInd+1);
            this->dimsPerBlock[pageInd] = make_pair(srowInd+1,scolInd);
        }
        srowInd++;
        if(srowInd == this->smallDim)
        {
            pagePrefix+=this->bigDim;
            srowInd = 0;
        }
    }

    return true;
}

vector<vector<int>> transposeData(vector<vector<int>> data)
{
    vector<vector<int>> result;
    int n = data.size();
    int m = data[0].size();
    vector<int> row(n, 0);
    result.assign(m, row);

    for(int i=0;i<n;i++)
        for(int j=0;j<m;j++)
            result[j][i] = data[i][j];
    return result;
}
void Matrix::transpose()
{
    // cout<<"here"<<endl;
    logger.log("Matrix::transpose");
    cout<<this->bigDim<<endl;
    for(int i=0; i< this->bigDim ; i++)
    {
        for(int j=i; j<this->bigDim; j++)
        {
            int pageAInd = i*this->bigDim + j;
            int pageBInd = j*this->bigDim + i;
            cout<<pageAInd<<" ; "<<pageBInd<<endl;
            if(pageAInd == pageBInd)
            {
                Cursor cursor(this->matrixName, pageAInd);
                vector<vector<int>> data = cursor.getWholePage();

                // transpose the data matrix
                vector<vector<int>> result = transposeData(data);
                bufferManager.writePage(this->matrixName, pageAInd, result, result.size());
            }
            else
            {
                Cursor cursor(this->matrixName, pageAInd);
                vector<vector<int>> dataA = cursor.getWholePage();

                cursor = Cursor(this->matrixName, pageBInd);
                vector<vector<int>> dataB = cursor.getWholePage();

                // transpose the data matrix
                vector<vector<int>> resultB = transposeData(dataA);
                vector<vector<int>> resultA = transposeData(dataB);

                bufferManager.writePage(this->matrixName, pageAInd, resultA, resultA.size());
                bufferManager.writePage(this->matrixName, pageBInd, resultB, resultB.size());
            }
        }
    }
}

/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
// void Matrix::makePermanent()
// {
//     logger.log("Table::makePermanent");
//     if(!this->isPermanent())
//         bufferManager.deleteFile(this->sourceFileName);
//     string newSourceFile = "../data/" + this->tableName + ".csv";
//     ofstream fout(newSourceFile, ios::out);
//
//     //print headings
//     this->writeRow(this->columns, fout);
//
//     Cursor cursor(this->tableName, 0);
//     vector<int> row;
//     for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
//     {
//         row = cursor.getNext();
//         this->writeRow(row, fout);
//     }
//     fout.close();
// }
