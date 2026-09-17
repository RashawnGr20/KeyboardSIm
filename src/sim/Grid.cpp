#include "sim/Grid.hpp"

#include <algorithm>

namespace ksim {

Grid::Grid(int nx, int ny)
    : nx_(nx),
      ny_(ny),
      buf_a_(static_cast<std::size_t>(nx) * ny, 0.0f),
      buf_b_(static_cast<std::size_t>(nx) * ny, 0.0f),
      buf_c_(static_cast<std::size_t>(nx) * ny, 0.0f),
      prev_(buf_a_.data()),
      curr_(buf_b_.data()),
      next_(buf_c_.data()) {}

void Grid::advance() {
    float* old_prev = prev_;
    prev_ = curr_;
    curr_ = next_;
    next_ = old_prev;
}

void Grid::zero_all() {
    std::fill(buf_a_.begin(), buf_a_.end(), 0.0f);
    std::fill(buf_b_.begin(), buf_b_.end(), 0.0f);
    std::fill(buf_c_.begin(), buf_c_.end(), 0.0f);
}

} 
