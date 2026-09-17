#include "sim/WaveSolver.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdexcept>

namespace ksim {

namespace {

double max_stable_dt(double c, double hx, double hy) {
    
    const double inv_h_sq = 1.0 / (hx * hx) + 1.0 / (hy * hy);
    return 1.0 / (c * std::sqrt(inv_h_sq));
}

} 

WaveSolver::WaveSolver(const Params& p) : p_(p) {
    if (p.nx < 3 || p.ny < 3) {
        throw std::runtime_error(
            "WaveSolver: grid too small — need at least 3x3 for a 5-point stencil");
    }
    if (p.hx <= 0.0 || p.hy <= 0.0) {
        throw std::runtime_error("WaveSolver: hx and hy must be positive");
    }
    if (p.c <= 0.0) {
        throw std::runtime_error("WaveSolver: wave speed c must be positive");
    }
    if (p.gamma < 0.0) {
        throw std::runtime_error("WaveSolver: gamma must be nonnegative");
    }
    if (p.cfl_safety <= 0.0 || p.cfl_safety > 1.0) {
        throw std::runtime_error(
            "WaveSolver: cfl_safety must lie in (0, 1] — anything > 1 "
            "violates stability by construction");
    }

    dt_ = p.cfl_safety * max_stable_dt(p.c, p.hx, p.hy);

    
    const double ax = (p.c * dt_) / p.hx;
    const double ay = (p.c * dt_) / p.hy;
    alpha_x_ = static_cast<float>(ax * ax);
    alpha_y_ = static_cast<float>(ay * ay);
    dt_sq_   = static_cast<float>(dt_ * dt_);

    // Damping-aware leapfrog:
    //   u^{n+1}(1+γΔt) = 2u^n − (1−γΔt)u^{n-1} + Δt²·[c²∇²u^n + f^n]
    // Precompute β = 1/(1+γΔt) and (1−γΔt) so the inner loop stays cheap.
    const double gdt = p.gamma * dt_;
    damp_beta_  = static_cast<float>(1.0 / (1.0 + gdt));
    damp_decay_ = static_cast<float>(1.0 - gdt);
}

void WaveSolver::init_from_rest(Grid& g) {

    std::memcpy(g.prev(), g.curr(), g.size() * sizeof(float));
}

void WaveSolver::step(Grid& g) {
    const int nx = g.nx();
    const int ny = g.ny();
    const float ax = alpha_x_;
    const float ay = alpha_y_;
    const float beta  = damp_beta_;
    const float decay = damp_decay_;

    const float* prev = g.prev();
    const float* curr = g.curr();
    float*       next = g.next();

    for (int j = 1; j < ny - 1; ++j) {
        const int row      = j * nx;
        const int row_up   = (j + 1) * nx;
        const int row_down = (j - 1) * nx;
        for (int i = 1; i < nx - 1; ++i) {
            const int c_idx = row + i;
            const float c   = curr[c_idx];
            const float e   = curr[c_idx + 1];
            const float w   = curr[c_idx - 1];
            const float n   = curr[row_up + i];
            const float s   = curr[row_down + i];
            const float p   = prev[c_idx];

            next[c_idx] = beta * (2.0f * c - decay * p
                                + ax * (e + w - 2.0f * c)
                                + ay * (n + s - 2.0f * c));
        }
    }

    g.advance();
    ++step_count_;
}

void WaveSolver::step(Grid& g, const float* forcing_field) {
    if (forcing_field == nullptr) {
        step(g);
        return;
    }

    const int nx = g.nx();
    const int ny = g.ny();
    const float ax = alpha_x_;
    const float ay = alpha_y_;
    const float dt2 = dt_sq_;
    const float beta  = damp_beta_;
    const float decay = damp_decay_;

    const float* prev = g.prev();
    const float* curr = g.curr();
    float*       next = g.next();

    for (int j = 1; j < ny - 1; ++j) {
        const int row      = j * nx;
        const int row_up   = (j + 1) * nx;
        const int row_down = (j - 1) * nx;
        for (int i = 1; i < nx - 1; ++i) {
            const int c_idx = row + i;
            const float c = curr[c_idx];
            const float e = curr[c_idx + 1];
            const float w = curr[c_idx - 1];
            const float n = curr[row_up + i];
            const float s = curr[row_down + i];
            const float p = prev[c_idx];
            next[c_idx] = beta * (2.0f * c - decay * p
                                + ax * (e + w - 2.0f * c)
                                + ay * (n + s - 2.0f * c)
                                + dt2 * forcing_field[c_idx]);
        }
    }

    g.advance();
    ++step_count_;
}

float WaveSolver::rms(const Grid& g) {
    const std::size_t n = g.size();
    const float* u = g.curr();
    double acc = 0.0;
    for (std::size_t k = 0; k < n; ++k) {
        const double v = u[k];
        acc += v * v;
    }
    return static_cast<float>(std::sqrt(acc / static_cast<double>(n)));
}

} 
