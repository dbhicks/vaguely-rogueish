/*************************************************************************
 *  Name: David Bacher-Hicks
 *  Date: 1 October 2016
 *  Description: Utilities for common tasks to be reused throughout 
 *  programming assignments in CS 162. Common tasks include input
 *  validation and cleaning.
 ************************************************************************/

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "utils.hpp"

const char INT_FAILPROMPT[] = "Please enter an integer number.";
const char DBL_FAILPROMPT[] = "Please enter a decimal number.";


    //////////////////////////////////////////////////////////////
   //           Input Validation / Cleaning                    //
  //////////////////////////////////////////////////////////////
  

/*************************************************************************
 * Display a prompt, then accept user input. The expected input is an integer.
 * Loop while there are any input errors and flush the buffer of any
 * extraneous input. If failPrompt is set to true (default), then display 
 * INT_FAILPROMPT in addition to the standard prompt on read errors.
 ************************************************************************/
int getInt(char const* prompt, bool failPrompt)
{
  int value = 0;

  if (prompt){
    std::cout << prompt;
  }
  while(!(std::cin >> value)){
    if (failPrompt){
      std::cout << INT_FAILPROMPT << std::endl;
    }
    if(prompt){
      std::cout << prompt;
    }
    std::cin.clear();
    flushKBB();
  }
  flushKBB();

  return value;
}


/*************************************************************************
 * Display a prompt, then accept user input. The expected input is a double.
 * Loop while there are any input errors and flush the buffer of any
 * extraneous input. If failPrompt is set to true (default), then display
 * DBL_FAILPROMPT in addition to the standard prompt on read errors.
 ************************************************************************/
double getDbl(char const* prompt, bool failPrompt)
{
  double value = 0;

  if (prompt){
    std::cout << prompt;
  }
  while(!(std::cin >> value)){
    if (failPrompt){
      std::cout << DBL_FAILPROMPT << std::endl;
    }
    if(prompt){
      std::cout << prompt;
    }
    std::cin.clear();
    flushKBB();
  }
  flushKBB();

  return value;
}


/*************************************************************************
 * A function to get a character from an expected domain of characters (choices).
 *
 * Display a prompt, then accept user input. The expected input is a character
 * in a set domain of characters. Loop while there are any input errors then 
 * flush the buffer of any extraneous input. If failPrompt is set to true 
 * (default) then list the characters in the acceptable domain (choices).
 ************************************************************************/
char getChar(std::string prompt, std::string choices, bool failPrompt)
{
  char value = '\0';
  std::cout << prompt;
  while(!(std::cin >> value) || (choices != "" && choices.find(value,0) == std::string::npos)){
    if(prompt != ""){
      std::cout << prompt;
      if (failPrompt){
        std::cout << '(' << split(choices, ',') << ") ";
      }
    }
    std::cin.clear();
    flushKBB();
  }
  flushKBB();

  return value;
}


/*************************************************************************
 * Flush characters from the input buffer until a newline is encountered.
 * cin.ignore performs similarly, but when cin.ignore calls are stacked,
 * they can result in unwanted keypress pauses. This function seems to solve
 * that problem.
 ************************************************************************/
void flushKBB()
{
  while(std::cin.peek() != '\n'){
    std::cin.get();
  }
}

/*************************************************************************
 * Same as flushKBB, except that it removes the newline character, while
 * flushKBB leaves it.
 ************************************************************************/
void emptyKBB()
{
  while(std::cin.get() != '\n'){}
}

    //////////////////////////////////////////////////////////////
   //                 String Manipulation                      //
  //////////////////////////////////////////////////////////////

/*************************************************************************
 * Return a string of characters in str, separated by splitChar.
 ************************************************************************/
std::string split(std::string str, char splitChar)
{
  unsigned int i;
  std::string outStr = "";
  for (i=0; i<str.length()-1; i++){
    outStr += str[i];
    outStr += splitChar;
  }
  outStr += str[i];
  return outStr;
}


    //////////////////////////////////////////////////////////////
   //                 Program Flow Control                     //
  //////////////////////////////////////////////////////////////

/*************************************************************************
 * A function to display a menu based on the passed instructions, choices,
 * and prompt. The menu function utilizes a restricted domain getChar() call
 * to repeatedly prompt the user for input until an acceptable choice
 * (a choice within the supplied domain of menuChoices) is entered by the user.
 * This choice is then returned.
 ************************************************************************/
char displayMenu(const std::string &menuInstructions, const std::string &menuChoices, const std::string &menuPrompt)
{
  std::system("clear");
  std::cout << menuInstructions;
  char menuChoice = getChar(menuPrompt, menuChoices);
  return menuChoice;
}

/*************************************************************************
 * A function to pause program flow and await a newline (enter) character.
 ************************************************************************/
void pauseForEnter()
{
  std::cout << "Press enter to continue..."; 
  if (std::cin.peek() == '\n'){
    std::cin.get();
  }
  std::cin.get();
}


    //////////////////////////////////////////////////////////////
   //                 Filesystem  /  i/o                       //
  //////////////////////////////////////////////////////////////

/*************************************************************************
 *  A function to return whether or not a file exists. Accepts a relative
 *  path as a parameter.
 ************************************************************************/
bool file_exists(const char *path)
{
  std::ifstream test_file(path);
  bool exists = false;

  if (test_file){
    exists = true;
    test_file.close();
  } 

  return exists;
}
