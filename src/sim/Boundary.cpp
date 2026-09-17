#include "sim/Boundary.hpp"

namespace ksim {

void DirichletBoundary::apply(Grid& g) const {
    const int nx = g.nx();
    const int ny = g.ny();
    float* u = g.curr();

    for (int i = 0; i < nx; ++i) {
        u[i]                            = 0.0f;   
        u[static_cast<std::size_t>(ny - 1) * nx + i] = 0.0f;   
    }
    for (int j = 0; j < ny; ++j) {
        u[static_cast<std::size_t>(j) * nx]              = 0.0f;   
        u[static_cast<std::size_t>(j) * nx + (nx - 1)]   = 0.0f;   
    }
}

} 
