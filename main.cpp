#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

#include "Floor.hpp"
#include "Character.hpp"
#include "Game.hpp"

int main()
{
  srand(time(0));

  initscr();
  clear();
  keypad(stdscr, TRUE);

  char name[26];
  
  printw( "What will your hero be named?\n" );
  getnstr(name, 25);

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
