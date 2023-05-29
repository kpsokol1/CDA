//Author: Kyle Sokol
//CWID: 12052043
//Date Completed: 9/18/2021
//Description: This program implements a circular dynamic array

#include <time.h>  //used to set random seed for Select method

using namespace std;

template <class T>
class CDA {
private:
    //member variables
    T* dataArray = nullptr; //array that will hold the raw data
    int *secondArray = nullptr; //secondArray will be used in conjunction with thirdArray to keep track of initialized values
    int *thirdArray = nullptr; //secondArray will be used in conjunction with thirdArray to keep track of initialized values

    int size = 0; //how many spots in the array that hold user data
    int capacity = 1; //how many pieces of data the array can hold
    int thirdArraySize = 0;

    int frontIndex = 0; //front index of circular array
    int backIndex = 0; //back index of circular array

    bool usingInitialization = false; //Determines if we need to look at our 2nd and 3rd arrays for initialization
    T backupValue; //will be returned if we need to return a reference to a non-valid index in the array
    T initializationValue; //will be set in the constructor that takes an initialization value and returned if dataArray[i] is not initialized
    int originalSize = 0; //used for the initialization value setup to know how many initialization values we need to keep track of

    //private methods
    int recursiveBinarySearch(T a[], int leftIndex, int rightIndex, int size, T e);

    void merge_MergeSort(T* a, T a1[], T a2[], int size);

    void sort_MergeSort(T* a, int size);

    void secondary_sort_MergeSort(T* a, int size);

    T secondarySelect(T* array, int size, int k);

    void doubleArrayCapacity(bool addFront);

    void halveArrayCapacity();

    bool isInitializedIndex(int index);

public:
    //public methods
    CDA();

    explicit CDA(int s);

    CDA(int s, T init);

    ~CDA();

    CDA(const CDA &oldArray);

    CDA &operator=(const CDA &oldArray);

    T &operator[](int i);

    void AddEnd(T v);

    void AddFront(T v);

    void DelEnd();

    void DelFront();

    int Length();

    int Capacity();

    T Select(int k);

    void Sort();

    int Search(T e);

    int BinSearch(T e);

    void printDataArray();
};

//constructors
template <class T>
CDA<T>::CDA(){
    dataArray = new T[1];
    capacity = 1;
}

template <class T>
CDA<T>::CDA(int s){
    dataArray = new T[s];
    secondArray = new int[s];
    thirdArray = new int[s];

    size = capacity = s;

    frontIndex = 0;
    backIndex = capacity - 1;
}

template <class T>
CDA<T>::CDA(int s, T init){
    dataArray = new T[s];
    secondArray = new int[s];
    thirdArray = new int[s];

    size = capacity = originalSize = s;
    usingInitialization = true;
    initializationValue = init;

    frontIndex = 0;
    backIndex = capacity - 1;
}

//destructor which is used to deallocate dynamic memory
template <class T>
CDA<T>::~CDA(){
    delete[] dataArray;
    delete[] secondArray;
    delete[] thirdArray;
}

//copy constructor (used when want to initialize a new object with an existing object's values)
template <class T>
CDA<T>::CDA(const CDA& oldArray){
    //copy over non-dynamic data
    size = oldArray.size;
    capacity = oldArray.capacity;
    thirdArraySize = oldArray.thirdArraySize;
    originalSize = oldArray.originalSize;

    frontIndex = oldArray.frontIndex;
    backIndex = oldArray.backIndex;

    backupValue = oldArray.backupValue;

    usingInitialization = oldArray.usingInitialization;
    if(usingInitialization){
        initializationValue = oldArray.initializationValue;
    }

    //create new arrays
    T* tempDataArray = new T[capacity];

    //copy data in old array to new array
    for(int i = 0; i < capacity; i++){
        tempDataArray[i] = oldArray.dataArray[i];
    }

    //copy data over for initialization arrays if needed
    if(usingInitialization == true){
        int* tempSecondArray = new int[originalSize];
        int* tempThirdArray = new int[originalSize];
        for(int i = 0; i < originalSize; i++){
            tempSecondArray[i] = oldArray.secondArray[i];
            tempThirdArray[i] = oldArray.thirdArray[i];
        }
        secondArray = tempSecondArray;
        thirdArray = tempThirdArray;
    }
    else{ //free up unnecessary memory
        delete[] secondArray;
        delete[] thirdArray;
    }
    dataArray = tempDataArray;
}

//copy assignment operator (used when you want to set the value of an existing object to the values of another initialized object)
//FIXME: Does not handle self-assignment properly
template <class T>
CDA<T>& CDA<T>::operator = (const CDA& oldArray){
    //copy over non-dynamic data
    size = oldArray.size;
    capacity = oldArray.capacity;
    thirdArraySize = oldArray.thirdArraySize;
    originalSize = oldArray.originalSize;

    frontIndex = oldArray.frontIndex;
    backIndex = oldArray.backIndex;

    backupValue = oldArray.backupValue;

    usingInitialization = oldArray.usingInitialization;
    if(usingInitialization){
        initializationValue = oldArray.initializationValue;
    }

    //create new arrays
    T* tempDataArray = new T[capacity];

    //copy data in old array to new array
    for(int i = 0; i < capacity; i++){
        tempDataArray[i] = oldArray.dataArray[i];
    }

    //copy data over for initialization arrays if needed
    if(usingInitialization == true){
        int* tempSecondArray = new int[originalSize];
        int* tempThirdArray = new int[originalSize];
        for(int i = 0; i < originalSize; i++){
            tempSecondArray[i] = oldArray.secondArray[i];
            tempThirdArray[i] = oldArray.thirdArray[i];
        }
        secondArray = tempSecondArray;
        thirdArray = tempThirdArray;
    }
    else{ //free up unnecessary memory
        delete[] secondArray;
        delete[] thirdArray;
    }
    dataArray = tempDataArray;

    return *this;
}

//overload [] operator
template <class T>
T& CDA<T>::operator[](int i){
    //determine if the index is out of bounds
    if(i < 0 || i > size - 1){
        cout << "Index: " << i << " is out of bounds for size: " << size << endl;
        return backupValue;
    }

    //calculate the corresponding index in our circular array
    int internalIndex = (frontIndex + i) % capacity;

    if(usingInitialization){
        // good value
        if(thirdArraySize > 0 && secondArray[i] <= thirdArraySize - 1 && secondArray[i] >= 0 && thirdArray[secondArray[i]] == i) {
            return dataArray[internalIndex];
        }
        //value not taken up yet but "intitialized" go ahead and store the initialized value in the array because we need to return a reference to it
        else{
            //orient "pointers" correctly for 2nd and 3rd arrays
            thirdArraySize++;
            secondArray[i] = thirdArraySize - 1;
            thirdArray[thirdArraySize - 1] = i;

            dataArray[internalIndex] = initializationValue; //put the initialized value in the dataArray
            return dataArray[internalIndex];
        }
    }
    return dataArray[internalIndex]; //return whatever is there (not using initialization so don't care)
}

template <class T>
void CDA<T>::AddEnd(T v){
    //double capacity of array if necessary
    if(size >= capacity){
        doubleArrayCapacity(false);  //True for AddFront(), False for AddEnd()
    }
    //add new element to back of array
    if(size > 0){
        backIndex = (backIndex + 1) % capacity; //increment back index in circular fashion
    }
    else{
        frontIndex = 0;
        backIndex = 0; //don't need to change backIndex if nothing in array to begin with
    }

    dataArray[backIndex] = v; //add the element to the back of the array

    if(usingInitialization && backIndex <= originalSize - 1){
        //put index of thirdArray in correct position in secondArray
        if(size != 0){
            secondArray[backIndex] = size - 1;
        }
        else{
            secondArray[backIndex] = size; //Could probably just say = to 0
        }
        thirdArraySize++;
        thirdArray[thirdArraySize] = backIndex;
    }
    //increment size of array
    size++;
}

template <class T>
void CDA<T>::AddFront(T v){
    //double capacity of array if necessary
    if(size >= capacity){
        doubleArrayCapacity(true);
    }
    else{
        //change front index
        frontIndex = ((frontIndex + capacity) - 1) % capacity; //could be (front - 1) % capacity but C++ weird
    }

    if(size == 0){
        frontIndex = 0;
        backIndex = 0;
    }

    //add new element to front of array
    dataArray[frontIndex] = v;

    if(usingInitialization == true){
        //put index of thirdArray in correct position in secondArray
        secondArray[frontIndex] = thirdArraySize;
        thirdArray[thirdArraySize] = frontIndex;
        thirdArraySize++;
    }

    //increment size of array
    size++;
}

template <class T>
void CDA<T>::DelEnd(){
    //Check to make sure there is something to delete first
    if(size > 0){
        //decrement size of array
        size--;

        //just mark space to be overwritten, so move back index accordingly
        backIndex = ((backIndex + capacity) - 1) % capacity;

        //change size of array if get to 1/4 capacity (make sure capacity doesn't go below 4)
        //cut capacity by 1/2
        //make new array of 1/4 size and copy data to it
        if(size <= capacity / 4){
            if((capacity) / 2 >= 4){
                halveArrayCapacity();
            }
            //if smaller than 8 then we don't need to change the array
        }
    }
}

template <class T>
void CDA<T>::DelFront(){
    //Check to make sure there is something to delete first
    if(size > 0){
        //decrement size of array
        size--;
        //just mark space to be overwritten, so calculate new front
        frontIndex = (frontIndex + 1) % capacity;

        //change size of array if get to 1/4 capacity (make sure capacity doesn't go below 4)
        //cut capacity by 1/2
        //make new array of 1/4 size and copy data to it
        if(size <= capacity / 4){
            if((capacity) / 2 >= 4){
                halveArrayCapacity();
            }
            //if smaller than 8 then we don't need to change the array
        }
    }
}

template <class T>
int CDA<T>::Length(){
    return size;
}

template <class T>
int CDA<T>::Capacity(){
    return capacity;
}

//selects the kth smallest element in the dynamic array (1 = smallest, size = largest) (be careful to only select between front and back index if array isn't full)
template <class T>
T CDA<T>::Select(int k){
    k = k - 1; //changing to 0 based indexing

    //generate random index for the pivot and set the seed using the current time
    srand(time(NULL)); //set random seed
    int randomIndex = (frontIndex + (rand() % size)) % capacity;

    T pivot = dataArray[randomIndex]; //picks a random index between front and back index
    if(!isInitializedIndex(randomIndex)){
        pivot = initializationValue;
    }

    //setup arrays to hold lessThan, greaterThan, and equalToPivot
    T* less = new T[size];
    T* equal = new T[size];
    T* greater = new T[size];
    int lessSize = 0;
    int equalSize = 0;
    int greaterSize = 0;

    if(!usingInitialization){
        for (int i = 0; i < size; i++){
            //place each element from the data array into arrays which contain elements equal in value to the pivot, less than, and greater than
            //less than pivot
            if ((dataArray[(frontIndex + i) % capacity]) < pivot) {
                less[lessSize] = dataArray[(frontIndex + i) % capacity];
                lessSize++;
            }
                //equal to pivot
            else if ((dataArray[(frontIndex + i) % capacity]) == pivot) {
                equal[equalSize] = dataArray[(frontIndex + i) % capacity];
                equalSize++;
            }
                //greater than pivot
            else {
                greater[greaterSize] = dataArray[(frontIndex + i) % capacity];
                greaterSize++;
            }
        }
    }
    else{
        for (int i = 0; i < size; i++){
            if(isInitializedIndex((frontIndex + i) % capacity)){
                //place each element from the data array into arrays which contain elements equal in value to the pivot, less than, and greater than
                //less than pivot
                if ((dataArray[(frontIndex + i) % capacity]) < pivot) {
                    less[lessSize] = dataArray[(frontIndex + i) % capacity];
                    lessSize++;
                }
                    //equal to pivot
                else if ((dataArray[(frontIndex + i) % capacity]) == pivot) {
                    equal[equalSize] = dataArray[(frontIndex + i) % capacity];
                    equalSize++;
                }
                    //greater than pivot
                else {
                    greater[greaterSize] = dataArray[(frontIndex + i) % capacity];
                    greaterSize++;
                }
            }
            else{
                //place each element from the data array into arrays which contain elements equal in value to the pivot, less than, and greater than
                //less than pivot
                if (initializationValue < pivot) {
                    less[lessSize] = initializationValue;
                    lessSize++;
                }

                    //equal to pivot
                else if (initializationValue == pivot) {
                    equal[equalSize] = initializationValue;
                    equalSize++;
                }

                    //greater than pivot
                else {
                    greater[greaterSize] = initializationValue;
                    greaterSize++;
                }
            }
        }
    }

    //k is in the lower numbers
    if (k < lessSize){
        return secondarySelect(less, lessSize, k);
    }
        //k is the pivot
    else if (k < lessSize + equalSize){
        if (isInitializedIndex(randomIndex)){
            return pivot;
        }
        return initializationValue;
    }
        //k is in the larger numbers
    else{
        return secondarySelect(greater, greaterSize, k - lessSize - equalSize);
    }
}

//Merge Sort
template <class T>
void CDA<T>::Sort(){
    sort_MergeSort(dataArray, size);
}

template <class T>
int CDA<T>::Search(T e){
    for (int i = 0; i < size; i++){
        if(usingInitialization){
            if(isInitializedIndex((frontIndex + i) % capacity)){
                if (e == dataArray[(frontIndex + i) % capacity]){
                    return i;
                }
            }
            else{
                if(e == initializationValue){
                    return i;
                }
            }
        }
        if (e == dataArray[(frontIndex + i) % capacity]){
            return i;
        }

    }
    return -1;
}

template <class T>
int CDA<T>::BinSearch(T e){
    //int leftIndex = (frontIndex + 0) % capacity;
    //int rightIndex = (frontIndex + size - 1) % capacity;
    return recursiveBinarySearch(dataArray,0, size - 1, size, e);
}

template<class T>
int CDA<T>::recursiveBinarySearch(T a[], int leftIndex, int rightIndex, int size, T e) {
    //check to see if element is in the array
    if (rightIndex >= leftIndex) {
        int middleIndex = (leftIndex + rightIndex) / 2;

        if (a[middleIndex] == e) {
            return middleIndex;
        }
            //look in lower half since value at middle index too high
        else if (a[middleIndex] > e) {
            return recursiveBinarySearch(a, leftIndex, middleIndex - 1, size, e);
        }
        //look in upper half since value at middle index too low
        return recursiveBinarySearch(a, middleIndex + 1, rightIndex, size, e);
    }

    //element not found
    int numToReturn = 0;
    if(usingInitialization){
        if(!isInitializedIndex(0)){
            a[0] = initializationValue;
        }
        if(!isInitializedIndex(size -1)){
            a[size - 1] = initializationValue;
        }
        if(!isInitializedIndex(leftIndex)){
            a[leftIndex] = initializationValue;
        }
    }
    if (a[0] > e) {
        numToReturn = 0;
    } else if (a[size - 1] < e) {
        numToReturn = size;
    } else {
        //numToReturn = leftIndex; //looks at index above where the number should be
        if(e < a[leftIndex]){
            numToReturn = leftIndex;
        }
        else{
            numToReturn = leftIndex + 1;
        }
    }
    return ~numToReturn;
}

template<class T>
void CDA<T>::merge_MergeSort(T* a, T a1[], T a2[], int size) {
    int size1 = size / 2;
    int size2 = size - size / 2;
    int p1 = 0;
    int p2 = 0;
    int i = 0;

    while (p1 < size1 && p2 < size2) {
        a[i++] = a1[p1] < a2[p2] ? a1[p1++] : a2[p2++];
    }
    while (p1 < size1) {
        a[i++] = a1[p1++];
    }
    while (p2 < size2) {
        a[i++] = a2[p2++];
    }
    frontIndex = 0;
    backIndex = size - 1;
}

template<class T>
void CDA<T>::sort_MergeSort(T* a, int size) {
    if (size <= 1) {
        return;
    }
    T* a1 = new T[size / 2];
    T* a2 = new T[size - (size / 2)];

    //split the array in half
    for (int i = 0; i < size / 2; i++) {
        if(usingInitialization){
            if(isInitializedIndex((frontIndex + i) % capacity)){
                a1[i] = a[(frontIndex + i) % capacity];
            }
            else{
                a1[i] = initializationValue;
            }
        }
        else{
            a1[i] = a[(frontIndex + i) % capacity];
        }
    }

    for (int i = size / 2; i < size; i++) {
        if(usingInitialization){
            if(isInitializedIndex((frontIndex + i) % capacity)){
                a2[i - size / 2] = a[(frontIndex + i) % capacity];
            }
            else{
                a2[i - size / 2] = initializationValue;
            }
        }
        else{
            a2[i - size / 2] = a[(frontIndex + i) % capacity];
        }
    }

    //sort the two halves then merge them together
    usingInitialization = false; //at this point we have set any unitialized values to the init value
    secondary_sort_MergeSort(a1, size / 2); //we need a secondary mergersort because the arrays in the second recursion aren't circular
    secondary_sort_MergeSort(a2, size - size / 2);
    merge_MergeSort(a, a1, a2, size);
}

template<class T>
void CDA<T>::secondary_sort_MergeSort(T* a, int size) {
    if (size <= 1) {
        return;
    }
    T* a1 = new T[size / 2];
    T* a2 = new T[size - (size / 2)];

    //split the array in half
    for (int i = 0; i < size / 2; i++) {
        a1[i] = a[i];
    }

    for (int i = size / 2; i < size; i++) {
        a2[i - size / 2] = a[i];
    }

    //sort the two halves then merge them together
    secondary_sort_MergeSort(a1, size / 2);
    secondary_sort_MergeSort(a2, size - size / 2);
    merge_MergeSort(a, a1, a2, size);
}

template<class T>
T CDA<T>::secondarySelect(T* array, int arr_size, int k) {

    //set seed of random value and generate random pivot
    srand(time(NULL));
    int randomIndex = rand() % arr_size;

    T pivot = array[randomIndex]; //picks a random index between front and back index
    T* less = new T[arr_size];
    T* equal = new T[arr_size];
    T* greater = new T[arr_size];
    int lessSize = 0;
    int equalSize = 0;
    int greaterSize = 0;

    for (int i = 0; i < arr_size; i++) {
        //place each element from the data array into arrays which contain elements equal in value to the pivot, less than, and greater than
        //less than pivot
        if ((array[i]) < pivot) {
            less[lessSize] = array[i];
            lessSize++;
        }
            //equal to pivot
        else if (array[i] == pivot) {
            equal[equalSize] = array[i];
            equalSize++;
        }
            //greater than pivot
        else {
            greater[greaterSize] = array[i];
            greaterSize++;
        }
    }
    //k is in the lower numbers
    if (k < lessSize) {
        return secondarySelect(less, lessSize, k);
    }
        //k is the pivot
    else if (k < lessSize + equalSize) {
        return pivot;
    }
        //k is in the larger numbers
    else {
        return secondarySelect(greater, greaterSize, k - lessSize - equalSize);
    }
}

//helper method to double the capacity of the array
template<class T>
void CDA<T>::doubleArrayCapacity(bool addFront) {
    //make new arrays and copy old data to new arrays
    T *tempDataArray = nullptr;

    tempDataArray = new T[capacity * 2];
    //copy data in old array(s) to new array(s) //leave space at front if add front
    if(addFront){
        for (int i = 0; i < size; i++) {
            tempDataArray[i + 1] = dataArray[(frontIndex + i) % size];
        }
        backIndex = size;
    }
    else{
        for (int i = 0; i < size; i++) {
            tempDataArray[i] = dataArray[(frontIndex + i) % size];
        }
        backIndex = size - 1;
    }

    frontIndex = 0;


    //double capacity
    capacity *= 2;


    //assign original pointers to the new larger arrays
    dataArray = tempDataArray;
}

template<class T>
void CDA<T>::halveArrayCapacity() {
    //make new arrays and copy old data to new arrays
    T *tempDataArray = nullptr;

    tempDataArray = new T[capacity / 2];
    //copy data in old array(s) to new array(s)
    for (int i = 0; i < size; i++) {
        if(usingInitialization){
            if(isInitializedIndex((frontIndex + i) % capacity)){
                tempDataArray[i] = dataArray[(frontIndex + i) % capacity]; //reset front back to index 0 in new array
            }
            else{
                tempDataArray[i] = initializationValue;
            }
        }
        else{
            tempDataArray[i] = dataArray[(frontIndex + i) % capacity]; //reset front back to index 0 in new array
        }
    }
    usingInitialization = false; //at this point everything that wasn't initialized now is meaning every value in data array less than size is filled
    frontIndex = 0;
    backIndex = (capacity / 4) - 1;

    //halve capacity
    capacity /= 2;

    //assign original pointers to the new smaller arrays
    dataArray = tempDataArray;
}
template<class T>
void CDA<T>::printDataArray(){
    for(int i = 0; i < capacity; i++){
        cout << dataArray[i] << ",";
    }
    cout << endl;
    cout << "f: " << frontIndex << endl;
    cout << "b: " << backIndex << endl;
    cout << "s: " << Length() << endl;
    cout << "c: " << Capacity() << endl;
}

template<class T>
bool CDA<T>::isInitializedIndex(int index){
    // good value //don't care at this point because grown past original initialization
    if(index > originalSize - 1){
        return true;
    }
    //good value 3rd condtion checks correct pointing, other two check sizes so not accessign something we shouldn't
    else if(thirdArraySize > 0 && secondArray[index] <= thirdArraySize - 1 && secondArray[index] >= 0 && thirdArray[secondArray[index]] == index) {
        return true;
    }
    //value not taken up yet but "initialized" go ahead and store the initialized value in the array
    return false;
}