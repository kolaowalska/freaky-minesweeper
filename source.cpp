#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

int N;
int** address; // *(*(address+i)+j)  <=>  address[i][j]
int* expanse; // *(expanse+i)  <=>  expanse[i]
typedef struct {
    bool isVisited;
    bool toDelete;
} TrashNode;

TrashNode** trash;

int getRowLength(int n){
    return *(expanse + n);
}

int* getAddressRow(int n){
    return *(address + n);
}
int getAddressValue(int n, int m){
    return *(getAddressRow(n) + m);
}

TrashNode* getTrashRow(int n) {
    return trash[n];
}

void toss(int n, int m) {
    trash[n][m].toDelete = true;
}

bool isTossed(int n, int m) {
    return trash[n][m].toDelete;
}

void writeToRow(int* row, int index, int value){
    *(row + index) = value;
}

void deleteRow(int n){
    free(*(address + n)); 
    for (int i = n + 1; i < N; i++){
        *(address + i - 1) = *(address + i); 
        *(expanse + i - 1) = *(expanse + i); 
    }
    N--;
    int** tempAddress = (int**)realloc(address, N * sizeof(int*));
    if (tempAddress != NULL || N == 0) {
        address = tempAddress;
    } else {
        // handle realloc failure if needed
    }

    int* tempExpanse = (int*)realloc(expanse, N * sizeof(int));
    if (tempExpanse != NULL || N == 0) {
        expanse = tempExpanse;
    } else {
        // handle realloc failure if needed
    }
}

void deleteTrashRow(int n){
    free(trash[n]);
    for (int i = n + 1; i < N; i++) {
        trash[i - 1] = trash[i];
    }
    N--;
    trash = (TrashNode**)realloc(trash, N * sizeof(TrashNode*));
}

void compressRow(int n){
    int count = 0;
    for (int i = 0; i < getRowLength(n); i++){
        if (!trash[n][i].toDelete)
            count++;
    }
    if (count == getRowLength(n))
        return;
    if (count == 0) {
        deleteRow(n);
        deleteTrashRow(n);
        return;
    }
    int* newRow = (int*)malloc(count * sizeof(int));
    int writeIndex = 0;
    for (int readIndex = 0; readIndex < getRowLength(n); readIndex++){
        if (!trash[n][readIndex].toDelete){
            newRow[writeIndex++] = getAddressValue(n, readIndex);
        }
    }
    free(*(address+n));
    *(address + n) = newRow;
    *(expanse + n) = count;
}

void compressArray(int numberOfRows){
    for (int i = 0; i < numberOfRows; i++){
        compressRow(i);
    }
}

void minesweeper(int n, int m, int anchorValue, char direction) {
    if (n < 0 || n > N - 1 || m < 0 || m > getRowLength(n) - 1)
        return;
    int value = getAddressValue(n, m);
    if (value != anchorValue || trash[n][m].isVisited)
        return;
    trash[n][m].isVisited = true;
    if (direction != 'L') {
        minesweeper(n, m + 1, value, 'R');
    }
    if (direction != 'U') {
        minesweeper(n + 1, m, value, 'D');
    }
    if (direction != 'R') {
        minesweeper(n, m - 1, value, 'L');
    }
    if (direction != 'D') {
        minesweeper(n - 1, m, value, 'U');
    }
}

void yoursweeper(int n, int m, int anchorValue, char direction){
    if (n < 0 || n > N - 1 || m < 0 || m > getRowLength(n) - 1)
        return;
    int value = getAddressValue(n, m);
    if (value != anchorValue || trash[n][m].isVisited)
        return;
    trash[n][m].isVisited = true;
    if (direction != 'Q') {
        yoursweeper(n + 1, m + 1, value, 'C');
    }
    if (direction != 'E') {
        yoursweeper(n + 1, m - 1, value, 'Z');
    }
    if (direction != 'Z') {
        yoursweeper(n - 1, m + 1, value, 'E');
    }
    if (direction != 'C') {
        yoursweeper(n - 1, m - 1, value, 'Q');
    }
}

void print(){ // W
    for (int i = 0; i < N; ++i){
        for (int j = 0; j < getRowLength(i); ++j){
            printf("%d ", getAddressValue(i, j));
        } printf("\n");
    } printf("\n");
}

void swapRows(int m, int n) { // Z
    int tempExpanse = *(expanse + n);
    *(expanse + n) = *(expanse + m);
    *(expanse + m) = tempExpanse;

    int* tempAddress = (int*)malloc(*(expanse + m) * sizeof(int));
    memcpy(tempAddress, *(address + n), *(expanse + n) * sizeof(int));
    free(*(address + n));

    *(address + n) = (int*)malloc(*(expanse + m) * sizeof(int));
    memcpy(*(address + n), *(address + m), *(expanse + m) * sizeof(int));
    free(*(address + m));

    *(address + m) = tempAddress;
}

void palindromeRow(int n){ // P
    int l = getRowLength(n);
    *(expanse + n) = 2 * l;
    *(address + n) = (int*)realloc(*(address+n), 2 * l * sizeof(int));
    for (int i = 0; i < l; ++i){
        *(*(address + n) + (2 * l - 1) - i) = *(*(address + n) + i);
    }
}

void insertRow(int m, int n, int k){ // M
    int ln = *(expanse + n);
    int lm = *(expanse + m);

    int* temp = (int*)realloc(*(address + m), (lm + ln) * sizeof(int));
    if (temp == NULL)
        return;
    *(address + m) = temp;

    for (int i = lm + ln - 1; i >= ln + k; --i){ 
        *(*(address + m) + i) = *(*(address + m) + i - ln);
    }
    for (int i = k; i < ln + k; ++i){
        *(*(address + m) + i) = *(*(address + n) + i - k);
    }
    *(expanse + m) = lm + ln;
    deleteRow(n);
    deleteTrashRow(n);
}

void linearCross(int n, int m){ // C
    trash = (TrashNode**)malloc(N * sizeof(TrashNode*));
    for (int i = 0; i < N; i++){
        trash[i] = (TrashNode*)calloc(getRowLength(n), sizeof(TrashNode));
    }
    minesweeper(n, m, getAddressValue(n, m), 'X');
    compressArray(N);
    free(trash);
}

void diagonalCross(int n, int m){ // X
    trash = (TrashNode**)malloc(N * sizeof(TrashNode*));
    for (int i = 0; i < N; i++){
        trash[i] = (TrashNode*)calloc(getRowLength(n), sizeof(TrashNode));
    }
    yoursweeper(n, m, getAddressValue(n, m), 'X');
    compressArray(N);
    free(trash);
}

void splitRow(int n, int k) { // R
    int lessCount = 0, greaterCount = 0;
    for (int i = 0; i < getRowLength(n); i++) {
        if (getAddressValue(n, i) > k)
            greaterCount++;
        else
            lessCount++;
    }
    if (lessCount == 0 || greaterCount == 0)
        return;

    N++;
    address = (int **)realloc(address, N * sizeof(int *));
    expanse = (int *)realloc(expanse, N * sizeof(int));
    for (int i = N - 2; i > n; i--) {
        *(address + i + 1) = *(address + i);
        *(expanse + i + 1) = *(expanse + i);
    }

    *(address + (n + 1)) = (int*)malloc(lessCount * sizeof(int));
    *(expanse + (n + 1)) = lessCount;
    if (!*(address+(n + 1))) { 
        // handle allocation failure if needed
        return;
    }

    int bottomWriteIndex = 0;
    int topWriteIndex = 0;
    int* bottomRow = (int*)malloc(lessCount * sizeof(int));
    int* topRow = (int*)malloc(greaterCount * sizeof(int));

    for (int readIndex = 0; readIndex < getRowLength(n); readIndex++) {
        int temp = getAddressValue(n, readIndex);
        if (temp > k) {
            writeToRow(topRow, topWriteIndex++, getAddressValue(n, readIndex));
        } else {
            writeToRow(bottomRow, bottomWriteIndex++, getAddressValue(n, readIndex));
        }
    }

    free(*(address + n));
    free(*(address + n + 1));
    *(address + n) = topRow;
    *(address + n + 1) = bottomRow;
    *(expanse + n) = greaterCount;
}

