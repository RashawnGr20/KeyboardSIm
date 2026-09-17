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

 
    void step(Grid& g);


    void step(Grid& g, const float* forcing_field);

    static float rms(const Grid& g);

private:
    Params p_;
    double dt_;
    float  alpha_x_;
    float  alpha_y_;
    float  dt_sq_;        
    float  damp_beta_;     
    float  damp_decay_;    
    int    step_count_ = 0;
};

} 
