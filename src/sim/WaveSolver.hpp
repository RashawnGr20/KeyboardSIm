#pragma once

#include "sim/Grid.hpp"

namespace ksim {


class WaveSolver {
public:
    struct Params {
        int    nx;
        int    ny;
        double hx;
        double hy;
        double c;
        double cfl_safety;
        double gamma = 0.0;   // viscous damping coefficient (1/s); 0 = undamped
    };

    explicit WaveSolver(const Params& p);

    
    double dt()          const { return dt_; }
    double time()        const { return step_count_ * dt_; }
    int    step_count()  const { return step_count_; }
    int    nx()          const { return p_.nx; }
    int    ny()          const { return p_.ny; }

   
    static void init_from_rest(Grid& g);

    // Plain leapfrog step; no external forcing.
    void step(Grid& g);

    // Leapfrog step that also adds Δt² · f^n from a precomputed forcing
    // field (same shape as the grid, row-major, i-fast). Callers zero the
    // field and let each Excitation add_to() into it before this call.
    void step(Grid& g, const float* forcing_field);

    static float rms(const Grid& g);

private:
    Params p_;
    double dt_;
    float  alpha_x_;
    float  alpha_y_;
    float  dt_sq_;         // Δt² as fp32, for the forcing term
    float  damp_beta_;     // 1 / (1 + γ·Δt), multiplier on entire RHS
    float  damp_decay_;    // 1 − γ·Δt, multiplier on prev
    int    step_count_ = 0;
};

} 
