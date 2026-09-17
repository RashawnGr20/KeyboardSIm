#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "io/Config.hpp"
#include "sim/Grid.hpp"

namespace ksim {


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

} 
