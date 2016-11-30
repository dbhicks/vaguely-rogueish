/*************************************************************************
 * Name: David Bacher-Hicks
 * Date: 1 October 2016
 * Description: A header file for general purpose utility functions.
 ************************************************************************/


#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>

//Input validation / cleaning
int getInt(char const* prompt=NULL, bool failPrompt=true);
double getDbl(char const* prompt=NULL, bool failPrompt=true);
char getChar(std::string prompt="", std::string choices="", bool failPrompt=true);
void flushKBB();
void emptyKBB();

//String manipulation
std::string split(std::string str, char splitChar);

//Program flow control
char displayMenu(const std::string &menuInstructions = "", const std::string &menuChoices = "", const std::string &menuPrompt = "");
void pauseForEnter();

//Filesystem, i/o
bool file_exists(const char *path);

        //////////////////////////////////////////////////////////
       //   Array Searching and Sorting Template Functions     //
      //////////////////////////////////////////////////////////

/*************************************************************************
 * Function: lin_arr_search
 * Description: performs a linear search on an array of <class T> and
 *              returns the first index found.
 *
 * Parameters:  
 *              1) const T &value  -  Value being searched for
 *              2) const T arr[]   -  The array being searched
 *              3) const int &size -  The size of the array
 *
 * Pre-conditions: None
 * Post-conditions: None
 * Return: int - the index of the found value,
 *               returns -1 if not found
 ************************************************************************/
template <class T>
int lin_arr_search(const T &value, const T arr[], const int &size)
{ 
  int index = -1;

  for(int i = 0; ((i < size) && (index == -1)); i++){
    if(arr[i] == value){
      index = i;
    }
  }

  return index;
}


/*************************************************************************
 * Function: bin_arr_search
 * Description: performs a binary search on an array of <class T> and
 *              returns the first index found.
 *
 * Parameters: 
 *              1) const T &value  - Value being searched for
 *              2) const T arr[]   - Array being searched
 *              3) const int &size - Size of array
 *
 * Pre-conditions:  The array should have been sorted
 * Post-conditions: None
 * Return: int - the index of the found value
 *               returns -1 if not found
 ************************************************************************/
template <class T>
int bin_arr_search(const T &value, const T arr[], const int &size)
{
  int min   = 0,
      max   = size - 1,
      index = -1;

  while((index == -1) && (min <= max)){

    int mid = (max + min) / 2;

    if(arr[mid] < value){
      min = mid + 1;
    } else if (arr[mid] > value){
      max = mid - 1;
    } else {
      index = mid;
    }
  }

  return index; 
}


/*************************************************************************
 * Function: arr_select_sort
 * Description: performs a select sort on the passed array of <class T>
 *              sorts ascending
 *
 * Parameters:  
 *            1) T arr[]         - The array to sort
 *            2) const int &size - the size of the array
 *
 * Pre-conditions:  none
 * Post-conditions: the passed array will have been sorted
 * Return: none
 ************************************************************************/
template <class T>
void arr_select_sort(T arr[], const int &size)
{
  for(int i = 0; i < size - 1; i++){

    T min     = arr[i];
    int index = i;

    for(int j = i + 1; j < size; j++){
      if( arr[j] < min ){
        min = arr[j];
        index = j;
      }
    }

    if(index != i){
      arr[index] = arr[i];
      arr[i] = min;
    }
  }
}

#endif
