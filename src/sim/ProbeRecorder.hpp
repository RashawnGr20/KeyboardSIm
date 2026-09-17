#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "io/Config.hpp"
#include "sim/Grid.hpp"

namespace ksim {

// Logs u(x, y, t) for one or more fixed grid nodes to a CSV file, one row
// per record() call. Probe positions from Config are snapped to the nearest
// grid node at construction, so the per-step call is a couple of array
// reads and a printf.
class ProbeRecorder {
public:
    ProbeRecorder(const std::vector<ProbeSpec>& probes,
                  int nx, int ny,
                  double hx, double hy,
                  const std::filesystem::path& csv_path);

    void record(double t, const Grid& g);

    int probe_count()  const { return static_cast<int>(indices_.size()); }
    int record_count() const { return record_count_; }

private:
    struct SnappedProbe {
        std::size_t linear_idx;
        std::string name;
    };
    std::vector<SnappedProbe> indices_;
    std::ofstream out_;
    int record_count_ = 0;
};

} // namespace ksim
