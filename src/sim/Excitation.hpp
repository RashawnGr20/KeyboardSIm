#pragma once

#include "io/Config.hpp"

namespace ksim {

class Excitation {
public:
    struct Params {
        double x0;              // strike center x, meters
        double y0;              // strike center y, meters
        double amplitude;       // peak forcing (units of u_tt)
        double sigma;           // spatial Gaussian sigma, meters
        double duration;        // strike window length, seconds
        double start_time = 0;  // when the strike begins, seconds
        StrikeShape shape = StrikeShape::RaisedCosine;
    };

    explicit Excitation(const Params& p);

    // True while t is inside the strike window.
    bool is_active(double t) const;

    // Point-evaluate f(x, y, t). Zero outside the strike window.
    double sample(double x, double y, double t) const;

    // Accumulate this excitation into a preallocated Nx*Ny forcing field
    // (row-major, i-fast). Only touches nodes inside a ±3σ bounding box
    // around (x0, y0). If !is_active(t), does nothing.
    void add_to(float* forcing_field,
                int nx, int ny,
                double hx, double hy,
                double t) const;

    const Params& params() const { return p_; }

private:
    Params p_;
    double two_sigma_sq_;  // precomputed 2·σ²
};

} // namespace ksim
