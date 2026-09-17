#pragma once

#include <filesystem>
#include <string>

#include "sim/Grid.hpp"

namespace ksim {


class SnapshotWriter {
public:
    SnapshotWriter(std::filesystem::path output_dir, double hx, double hy);

    
    void write(const Grid& g, double t, int step);

    const std::filesystem::path& output_dir() const { return output_dir_; }
    int write_count() const { return write_count_; }

private:
    std::filesystem::path output_dir_;
    double hx_;
    double hy_;
    int    write_count_ = 0;
    bool   dir_ready_ = false;
};

} 
