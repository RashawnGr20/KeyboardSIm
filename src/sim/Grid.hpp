#pragma once

#include <cstddef>
#include <vector>

namespace ksim {


class Grid {
public:
    Grid(int nx, int ny);

    int nx() const { return nx_; }
    int ny() const { return ny_; }
    std::size_t size() const { return static_cast<std::size_t>(nx_) * ny_; }

  
    float*       prev()       { return prev_; }
    const float* prev() const { return prev_; }
    float*       curr()       { return curr_; }
    const float* curr() const { return curr_; }
    float*       next()       { return next_; }
    const float* next() const { return next_; }


    float& at_curr(int i, int j)             { return curr_[idx(i, j)]; }
    float  at_curr(int i, int j) const       { return curr_[idx(i, j)]; }
    float& at_prev(int i, int j)             { return prev_[idx(i, j)]; }
    float  at_prev(int i, int j) const       { return prev_[idx(i, j)]; }
    float& at_next(int i, int j)             { return next_[idx(i, j)]; }
    float  at_next(int i, int j) const       { return next_[idx(i, j)]; }

    std::size_t idx(int i, int j) const {
        return static_cast<std::size_t>(j) * nx_ + i;
    }

   
    void advance();

  
    void zero_all();

private:
    int nx_;
    int ny_;
    std::vector<float> buf_a_;
    std::vector<float> buf_b_;
    std::vector<float> buf_c_;
    float* prev_;
    float* curr_;
    float* next_;
};

} 
