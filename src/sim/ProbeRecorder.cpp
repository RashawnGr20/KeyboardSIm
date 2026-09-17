#include "sim/ProbeRecorder.hpp"

#include <cmath>
#include <stdexcept>

namespace ksim {

ProbeRecorder::ProbeRecorder(const std::vector<ProbeSpec>& probes,
                             int nx, int ny,
                             double hx, double hy,
                             const std::filesystem::path& csv_path) {
    indices_.reserve(probes.size());
    for (std::size_t k = 0; k < probes.size(); ++k) {
        const auto& p = probes[k];
        const int i = static_cast<int>(std::round(p.x / hx));
        const int j = static_cast<int>(std::round(p.y / hy));
        if (i < 0 || i >= nx || j < 0 || j >= ny) {
            throw std::runtime_error(
                "ProbeRecorder: probe '" + p.name + "' at (" +
                std::to_string(p.x) + ", " + std::to_string(p.y) +
                ") snaps outside the grid");
        }
        SnappedProbe sp;
        sp.linear_idx = static_cast<std::size_t>(j) * nx + i;
        sp.name = p.name.empty()
            ? ("probe" + std::to_string(k))
            : p.name;
        indices_.push_back(std::move(sp));
    }

    if (auto parent = csv_path.parent_path(); !parent.empty()) {
        std::error_code ec;
        std::filesystem::create_directories(parent, ec);
        if (ec) {
            throw std::runtime_error(
                "ProbeRecorder: cannot create parent directory " +
                parent.string() + ": " + ec.message());
        }
    }

    out_.open(csv_path);
    if (!out_) {
        throw std::runtime_error(
            "ProbeRecorder: cannot open " + csv_path.string() + " for writing");
    }

    out_ << "t";
    for (const auto& sp : indices_) out_ << "," << sp.name;
    out_ << "\n";
}

void ProbeRecorder::record(double t, const Grid& g) {
    const float* u = g.curr();
    out_ << t;
    for (const auto& sp : indices_) {
        out_ << "," << u[sp.linear_idx];
    }
    out_ << "\n";
    ++record_count_;
}

} // namespace ksim
