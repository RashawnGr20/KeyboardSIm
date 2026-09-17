#include "sim/Excitation.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <stdexcept>

namespace ksim {

namespace {


double envelope(double tau, double duration, StrikeShape shape) {
    if (tau < 0.0 || tau > duration) return 0.0;
    const double u = tau / duration;
    switch (shape) {
        case StrikeShape::RaisedCosine:
            return 0.5 * (1.0 - std::cos(2.0 * std::numbers::pi * u));
        case StrikeShape::HalfSine:
            return std::sin(std::numbers::pi * u);
    }
    return 0.0;
}

} 

Excitation::Excitation(const Params& p) : p_(p) {
    if (p.sigma <= 0.0) {
        throw std::runtime_error("Excitation: sigma must be positive");
    }
    if (p.duration <= 0.0) {
        throw std::runtime_error("Excitation: duration must be positive");
    }
    two_sigma_sq_ = 2.0 * p.sigma * p.sigma;
}

bool Excitation::is_active(double t) const {
    const double tau = t - p_.start_time;
    return tau >= 0.0 && tau <= p_.duration;
}

double Excitation::sample(double x, double y, double t) const {
    const double tau = t - p_.start_time;
    const double g = envelope(tau, p_.duration, p_.shape);
    if (g == 0.0) return 0.0;
    const double r2 = (x - p_.x0) * (x - p_.x0) + (y - p_.y0) * (y - p_.y0);
    return p_.amplitude * g * std::exp(-r2 / two_sigma_sq_);
}

void Excitation::add_to(float* forcing_field,
                        int nx, int ny,
                        double hx, double hy,
                        double t) const {
    if (!is_active(t)) return;
    const double tau = t - p_.start_time;
    const double g = envelope(tau, p_.duration, p_.shape);
    if (g == 0.0) return;


    const double half_box = 3.0 * p_.sigma;
    const int i_lo = std::max(0,      static_cast<int>(std::floor((p_.x0 - half_box) / hx)));
    const int i_hi = std::min(nx - 1, static_cast<int>(std::ceil ((p_.x0 + half_box) / hx)));
    const int j_lo = std::max(0,      static_cast<int>(std::floor((p_.y0 - half_box) / hy)));
    const int j_hi = std::min(ny - 1, static_cast<int>(std::ceil ((p_.y0 + half_box) / hy)));

    const double amp_g = p_.amplitude * g;

    for (int j = j_lo; j <= j_hi; ++j) {
        const double y  = j * hy;
        const double dy = y - p_.y0;
        const double dy2 = dy * dy;
        for (int i = i_lo; i <= i_hi; ++i) {
            const double x  = i * hx;
            const double dx = x - p_.x0;
            const double r2 = dx * dx + dy2;
            const double v  = amp_g * std::exp(-r2 / two_sigma_sq_);
            forcing_field[static_cast<std::size_t>(j) * nx + i] += static_cast<float>(v);
        }
    }
}

} 
