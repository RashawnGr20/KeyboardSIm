// Phase 1: run a full sim from a config file.

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <vector>

#include "io/Config.hpp"
#include "sim/Boundary.hpp"
#include "sim/Excitation.hpp"
#include "sim/Grid.hpp"
#include "sim/ProbeRecorder.hpp"
#include "sim/SnapshotWriter.hpp"
#include "sim/WaveSolver.hpp"

int main(int argc, char** argv) {
    try {
        ksim::Config cfg;
        if (argc >= 2) {
            cfg = ksim::Config::from_toml_file(argv[1]);
            std::printf("loaded config: %s\n", argv[1]);
        } else {
            std::printf("no config path given, using built-in defaults\n");
        }

        const double hx = cfg.lx / (cfg.nx - 1);
        const double hy = cfg.ly / (cfg.ny - 1);

        ksim::WaveSolver::Params sp{
            cfg.nx, cfg.ny, hx, hy, cfg.c, cfg.cfl_safety, cfg.gamma
        };
        ksim::WaveSolver solver(sp);

        ksim::Excitation::Params ep{
            cfg.strike_x, cfg.strike_y,
            cfg.strike_amplitude, cfg.strike_sigma,
            cfg.strike_duration,  0.0,
            cfg.strike_shape
        };
        ksim::Excitation excitation(ep);

        ksim::DirichletBoundary boundary;

        ksim::Grid g(cfg.nx, cfg.ny);           
        ksim::WaveSolver::init_from_rest(g);

        const auto run_dir  = std::filesystem::path(cfg.data_dir) / cfg.run_name;
        const auto snap_dir = run_dir / "snapshots";
        const auto probe_csv = run_dir / "probes.csv";
        ksim::SnapshotWriter writer(snap_dir, hx, hy);
        ksim::ProbeRecorder  probe_rec(cfg.probes, cfg.nx, cfg.ny, hx, hy, probe_csv);

        std::vector<float> forcing(static_cast<std::size_t>(cfg.nx) * cfg.ny, 0.0f);

        std::printf("grid   : %d x %d  (h = %.6g m, %.6g m)\n",
                    cfg.nx, cfg.ny, hx, hy);
        std::printf("physics: c = %.3f m/s, gamma = %.3f 1/s\n",
                    cfg.c, cfg.gamma);
        std::printf("time   : dt = %.6g s (CFL safety = %.2f), t_end = %.3f s\n",
                    solver.dt(), cfg.cfl_safety, cfg.t_end);
        std::printf("strike : (%.3f, %.3f)  A = %.3f  sigma = %.3g m  duration = %.3g s\n",
                    ep.x0, ep.y0, ep.amplitude, ep.sigma, ep.duration);

        const int total_steps = static_cast<int>(std::ceil(cfg.t_end / solver.dt()));
        std::printf("running %d steps, snapshots -> %s\n",
                    total_steps, snap_dir.string().c_str());
        std::printf("%8s  %12s  %12s\n", "step", "t (s)", "rms(u)");

        for (int n = 0; n <= total_steps; ++n) {
            const double t = n * solver.dt();

            probe_rec.record(t, g);

            if (n % cfg.snapshot_interval == 0) {
                std::printf("%8d  %12.6f  %12.6g\n",
                            n, t,
                            static_cast<double>(ksim::WaveSolver::rms(g)));
                writer.write(g, t, n);
            }
            if (n == total_steps) break;

            if (excitation.is_active(t)) {
                std::fill(forcing.begin(), forcing.end(), 0.0f);
                excitation.add_to(forcing.data(), cfg.nx, cfg.ny, hx, hy, t);
                solver.step(g, forcing.data());
            } else {
                solver.step(g);
            }
            boundary.apply(g);
        }
        std::printf("done. wrote %d snapshots, %d probe rows.\n",
                    writer.write_count(), probe_rec.record_count());
        return 0;
    } catch (const std::exception& ex) {
        std::fprintf(stderr, "error: %s\n", ex.what());
        return 1;
    }
}
