#pragma once

#include "io/Config.hpp"

namespace ksim {

class Excitation {
public:
    struct Params {
        double x0;              
        double y0;              
        double amplitude;       
        double sigma;           
        double duration;        
        double start_time = 0;  
        StrikeShape shape = StrikeShape::RaisedCosine;
    };

    explicit Excitation(const Params& p);

    
    bool is_active(double t) const;

    double sample(double x, double y, double t) const;


    void add_to(float* forcing_field,
                int nx, int ny,
                double hx, double hy,
                double t) const;

    const Params& params() const { return p_; }

private:
    Params p_;
    double two_sigma_sq_;  
};

} 
