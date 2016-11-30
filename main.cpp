#include <ncurses.h>
#include "Floor.hpp"
#include "Character.hpp"
#include "Game.hpp"

void move_player(Character *character, Floor &floor, direction d);

int main()
{
  initscr();
  clear();
  noecho();
  raw();
  curs_set(0);
  keypad(stdscr, TRUE);

  int input;
  Game game;

  while ( game.is_in_progress() ) {
    game.read_input( getch() );
    clear();
    printw( game.render().c_str() );
    refresh();
  }
  
  clear();
  printw("Press any key to continue...");
  refresh();
  getch(); 
  endwin();

  return 0;
}
