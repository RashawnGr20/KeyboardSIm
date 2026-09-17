#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace ksim {

enum class StrikeShape {
    RaisedCosine,
    HalfSine,
};

struct ProbeSpec {
    
    double x;
    double y;
    std::string name;
};


struct Config {
    // --- Grid ---
    int    nx = 128;
    int    ny = 128;
    double lx = 1.0;   
    double ly = 1.0;   

    // --- Physics ---
    double c     = 100.0;  
    double gamma = 0.0;    

    // --- Time ---
    double t_end             = 0.05; 
    int    snapshot_interval = 50;    
    double cfl_safety        = 0.9;   

    // --- Excitation ---
    double      strike_x        = 0.5;    
    double      strike_y        = 0.5;    
    double      strike_amplitude = 1.0;  
    double      strike_sigma    = 0.01;   
    double      strike_duration = 0.001;  
    StrikeShape strike_shape    = StrikeShape::RaisedCosine;

    std::vector<ProbeSpec> probes;


    std::string run_name  = "run";
    std::string data_dir  = "data";

  
    static Config from_toml_string(std::string_view text);
    static Config from_toml_file(const std::filesystem::path& path);
};


StrikeShape strike_shape_from_string(std::string_view s);
std::string_view to_string(StrikeShape s);

} 
