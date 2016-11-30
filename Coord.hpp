#ifndef COORD_HPP
#define COORD_HPP

class Coord{
  private:
    int x_coord;
    int y_coord;

  public:
    Coord() {}
    Coord(int x, int y) { this->x_coord = x, this->y_coord = y; }
    bool operator<(const Coord &right) const;

    void set(int x, int y) { this->x_coord = x; this->y_coord = y; }
    void set_x(int x) { this->x_coord = x; }
    void set_y(int y) { this->y_coord = y; }
    int x() const { return this->x_coord; }
    int y() const { return this->y_coord; }
};

#endif
