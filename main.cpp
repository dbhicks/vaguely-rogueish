#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

#include "Floor.hpp"
#include "Character.hpp"
#include "Game.hpp"

const char *INTRO_MESSAGE = 
  "Welcome to the Vaguely Roguelike Game\n\n"
  "You play the role of an adventurer who has been hired to\n"
  "find and exterminate a Minotaur that has taken up residence\n"
  "in a dungeon near a small village. Your employer has given\n"
  "you a time limit of 5 days to complete your task.\n\n"
  "Find keys, unlock doors, kill monsters, level up, and have fun!\n\n"
  "If you are having any trouble with the game, consult the game\n"
  "guide included in the provided documentation.\n\n"
  "At a minimum, be aware of the following controls:\n"
  "Q - quit the game\n"
  "w, a, s, d - move up, left, down, right\n"
  "arrow keys - same as w, a, s, d\n"
  "g - get items\n"
  "c - display character information\n"
  "i - open your inventory\n\n"
  "In order to attack monsters or open doors, issue a move command in that direction\n\n";

int main()
{
  srand(time(0));

  initscr();
  clear();
  keypad(stdscr, TRUE);

  char name[26];

  printw(INTRO_MESSAGE);
  printw("Press any key to continue...");
  refresh();
  getch();

  clear();
  printw( "What will your hero be named?\n" );
  refresh();
  getnstr(name, 25);
  clear();

  raw();
  noecho();
  curs_set(0);

  int input;
  Game game(name);
  printw( game.render().c_str() );
  refresh();
  
  while ( game.is_in_progress() ) {
    input = getch();
    game.read_input( input );
    clear();
    printw( game.render().c_str() );
    refresh();
  } 
  printw("Press any key to exit...");  
  refresh();
  getch(); 
  endwin();

  return 0;
}


