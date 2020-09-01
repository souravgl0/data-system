# Approach

### Page Layout
    Matrix of Dimension N*N is divided into a matrix of Blocks of size SmallDim * SmallDim

    Example:
    Assuming SmallDim = 2 (Only for example, In reality it is larger)
        11 51 65 98
        51 20 10 24
        20 15 48 10
        10 54 87 98

    is divided into
        11 51
        51 20

        65 98
        10 24

    and so on...

    Each Such block is strored in a page.

### Transpose Algorithm
    Consider a block (i,j) of the outer matrix
    We store it in the PageIndex = i*BigDim + j
    For Transposing, We transpose the page Individually. This can be done in memory.
    Now, This block is swapped with the Data Block at (j,i);

    Thus, we iterate through each i,j for the outer matrix,
    Transpose the blocks at (i,j) and (j,i) and swap them,
    For diagonal blocks, we dont' need to swap, only transpose.

# Calculations

 - Number of Data Blocks used to Store the whole Matrix
    ```
    Maximum Dimension of Square Matrix that can be stored in a Block: *SmallDim*
    SmallDim = sqrt((BLOCK_SIZE * 1000)/4)

    *BigDim* is the dimension of outer (bigger) Matrix (Whose each cell is a Data Block)
    BigDim = ceil(N/SmallDim)

    Total No. of Blocks:
        *N_BLOCKS* = BigDim * BigDim
    ```
 - Number of Data Blocks Needed To Access for Transpose Operation
     ```
     Each Block is accessed once.
     Therefore, Number of Data Block Accesses = N_BLOCKS
     ```
     #### However, For the Load Operation:
        Each row of the matrix has data spanned by upto BigDim blocks.
        Thus, In the worst case (where every page needs to be replaced), We are accessing each data block
        SmallDim number of times.
        Therefore, No of Data Block Accesses = N_BLOCKS * SmallDim

    #### Similarly, For Export Operation:
        No of Data Block Accesses = N_BLOCKS * SmallDim
