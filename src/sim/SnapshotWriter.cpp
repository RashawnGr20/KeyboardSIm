#include "sim/SnapshotWriter.hpp"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <stdexcept>

namespace ksim {

namespace {

constexpr char kMagic[8] = {'K','S','I','M','S','N','A','P'};
constexpr std::uint32_t kVersion = 1;

std::string step_filename(int step) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "snap_%08d.bin", step);
    return std::string(buf);
}

} // namespace

SnapshotWriter::SnapshotWriter(std::filesystem::path output_dir, double hx, double hy)
    : output_dir_(std::move(output_dir)), hx_(hx), hy_(hy) {}

void SnapshotWriter::write(const Grid& g, double t, int step) {
    if (!dir_ready_) {
        std::error_code ec;
        std::filesystem::create_directories(output_dir_, ec);
        if (ec) {
            throw std::runtime_error(
                "SnapshotWriter: cannot create directory " +
                output_dir_.string() + ": " + ec.message());
        }
        dir_ready_ = true;
    }

    const auto path = output_dir_ / step_filename(step);
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        throw std::runtime_error(
            "SnapshotWriter: cannot open file " + path.string());
    }

    const std::uint32_t nx_u = static_cast<std::uint32_t>(g.nx());
    const std::uint32_t ny_u = static_cast<std::uint32_t>(g.ny());
    const std::uint32_t step_u = static_cast<std::uint32_t>(step);

    out.write(kMagic, 8);
    out.write(reinterpret_cast<const char*>(&kVersion),  sizeof(kVersion));
    out.write(reinterpret_cast<const char*>(&nx_u),      sizeof(nx_u));
    out.write(reinterpret_cast<const char*>(&ny_u),      sizeof(ny_u));
    out.write(reinterpret_cast<const char*>(&hx_),       sizeof(hx_));
    out.write(reinterpret_cast<const char*>(&hy_),       sizeof(hy_));
    out.write(reinterpret_cast<const char*>(&t),         sizeof(t));
    out.write(reinterpret_cast<const char*>(&step_u),    sizeof(step_u));
    out.write(reinterpret_cast<const char*>(g.curr()),
              static_cast<std::streamsize>(g.size() * sizeof(float)));

    if (!out) {
        throw std::runtime_error(
            "SnapshotWriter: write failed for " + path.string());
    }
    ++write_count_;
}

} // namespace ksim
