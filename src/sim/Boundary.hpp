#pragma once

#include "sim/Grid.hpp"

namespace ksim {

class Boundary {
public:
    virtual ~Boundary() = default;
    virtual void apply(Grid& g) const = 0;
};

class DirichletBoundary final : public Boundary {
public:
    void apply(Grid& g) const override;
};

} // namespace ksim
