#include "io/Config.hpp"

#include <iostream>
#include <set>
#include <stdexcept>
#include <string>

#include <toml++/toml.hpp>

namespace ksim {

namespace {


template <typename T>
void assign_if_present(const toml::table& tbl, std::string_view key, T& out) {
    if (auto node = tbl.get(key)) {
        if (auto v = node->value<T>()) {
            out = *v;
        } else {
            throw std::runtime_error(
                std::string("config: field '") + std::string(key) +
                "' has wrong type");
        }
    }
}

// Sub-section access with a clear error on wrong type.
const toml::table* subtable(const toml::table& tbl, std::string_view key) {
    if (auto node = tbl.get(key)) {
        if (auto sub = node->as_table()) return sub;
        throw std::runtime_error(
            std::string("config: '") + std::string(key) + "' must be a table");
    }
    return nullptr;
}


void warn_unknown_top_keys(const toml::table& tbl) {
    static const std::set<std::string> known = {
        "grid", "physics", "time", "excitation", "probes", "output"
    };
    for (const auto& [key, _] : tbl) {
        if (known.find(std::string(key.str())) == known.end()) {
            std::cerr << "config: warning: unknown top-level key '"
                      << key.str() << "'\n";
        }
    }
}

Config parse(const toml::table& t) {
    Config cfg;
    warn_unknown_top_keys(t);

    if (auto* g = subtable(t, "grid")) {
        assign_if_present(*g, "nx", cfg.nx);
        assign_if_present(*g, "ny", cfg.ny);
        assign_if_present(*g, "lx", cfg.lx);
        assign_if_present(*g, "ly", cfg.ly);
    }

    if (auto* p = subtable(t, "physics")) {
        assign_if_present(*p, "c",     cfg.c);
        assign_if_present(*p, "gamma", cfg.gamma);
    }

    if (auto* tm = subtable(t, "time")) {
        assign_if_present(*tm, "t_end",             cfg.t_end);
        assign_if_present(*tm, "snapshot_interval", cfg.snapshot_interval);
        assign_if_present(*tm, "cfl_safety",        cfg.cfl_safety);
    }

    if (auto* e = subtable(t, "excitation")) {
        assign_if_present(*e, "strike_x",         cfg.strike_x);
        assign_if_present(*e, "strike_y",         cfg.strike_y);
        assign_if_present(*e, "strike_amplitude", cfg.strike_amplitude);
        assign_if_present(*e, "strike_sigma",     cfg.strike_sigma);
        assign_if_present(*e, "strike_duration",  cfg.strike_duration);
        if (auto node = e->get("strike_shape")) {
            if (auto s = node->value<std::string>()) {
                cfg.strike_shape = strike_shape_from_string(*s);
            }
        }
    }

    if (auto node = t.get("probes")) {
        if (auto arr = node->as_array()) {
            for (const auto& elem : *arr) {
                if (auto* row = elem.as_table()) {
                    ProbeSpec ps;
                    assign_if_present(*row, "x",    ps.x);
                    assign_if_present(*row, "y",    ps.y);
                    assign_if_present(*row, "name", ps.name);
                    cfg.probes.push_back(std::move(ps));
                }
            }
        } else {
            throw std::runtime_error("config: 'probes' must be an array");
        }
    }

    if (auto* o = subtable(t, "output")) {
        assign_if_present(*o, "run_name", cfg.run_name);
        assign_if_present(*o, "data_dir", cfg.data_dir);
    }

    return cfg;
}

} 

Config Config::from_toml_string(std::string_view text) {
    try {
        auto tbl = toml::parse(text);
        return parse(tbl);
    } catch (const toml::parse_error& err) {
        throw std::runtime_error(
            std::string("config: TOML parse error: ") + err.description().data());
    }
}

Config Config::from_toml_file(const std::filesystem::path& path) {
    try {
        auto tbl = toml::parse_file(path.string());
        return parse(tbl);
    } catch (const toml::parse_error& err) {
        throw std::runtime_error(
            std::string("config: TOML parse error in ") + path.string() + ": " +
            err.description().data());
    }
}

StrikeShape strike_shape_from_string(std::string_view s) {
    if (s == "raised_cosine") return StrikeShape::RaisedCosine;
    if (s == "half_sine")     return StrikeShape::HalfSine;
    throw std::runtime_error(
        std::string("config: unknown strike_shape '") + std::string(s) + "'");
}

std::string_view to_string(StrikeShape s) {
    switch (s) {
        case StrikeShape::RaisedCosine: return "raised_cosine";
        case StrikeShape::HalfSine:     return "half_sine";
    }
    return "unknown";
}

} 
