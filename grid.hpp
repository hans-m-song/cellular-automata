#ifndef GRID_H
#define GRID_H

#include <utility>
#include <vector>

typedef std::pair<int, int> Point;
typedef std::vector<int> Vector;
typedef std::vector<Vector> Vector2D;
enum Direction { N, NE, E, SE, S, SW, W, NW };

class Grid {
  // space dimensions
  int width;
  int height;

  // Space allocated for cell generations
  Vector2D generation;
  Vector2D next_generation;

  /**
   * Finds an empty cell within the space, only used for first generation cells
   * @return {Point}: point to an empty cell
   */
  Point empty_cell(void);

  /**
   * Finds the total number of live cells surrounding a point
   * @param {int} x: x coordinate of origin point
   * @param {int} y: y coordinate of origin point
   * @return {int} : total number of live cells
   */
  int sum_neighbour(int x, int y);

  /**
   * Allocates a two dimensional vector indexed as vector[width][height]
   * @return {Vector2D}: the newly allocated two dimensional vector
   */
  Vector2D allocate_space(void);

  /**
   * Creates a point by applying the cardinal directions to a point
   * @param {Point} origin       : point to apply direction to
   * @param {Direction} direction: a direction to apply
   * @return {Point}             : a new point, one unit from the origin point
   *                               in the given direction
   */
  Point apply_direction(Point origin, Direction direction);

  public:
  Grid(int width, int height, double initial_density);

  /**
   * Prints the current state of the grid
   * Note: clears the terminal screen
   */
  void print(void);

  /**
   * Simulates the life of a single generation
   */
  void tick(void);

  /**
   * Finds the total number of living cells currently alive
   * @return {int}: total number of living cells
   */
  int sum(void);
};

#endif