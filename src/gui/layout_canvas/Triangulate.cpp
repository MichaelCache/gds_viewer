#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <vector>

std::vector<std::array<std::pair<double, double>, 3>> triangulate2D(
    const double* polygon, int points) {
  std::vector<std::array<std::pair<double, double>, 3>> res;
  for (int i = 0; i < points; i++) {
    std::array<std::pair<double, double>, 3> triangle;
    triangle[0] = {polygon[i * 2], polygon[i * 2 + 1]};
    triangle[1] = {polygon[(i + 1) * 2], polygon[(i + 1) * 2 + 1]};
    triangle[2] = {polygon[(i + 2) * 2], *polygon};
  }
  return res;
}
