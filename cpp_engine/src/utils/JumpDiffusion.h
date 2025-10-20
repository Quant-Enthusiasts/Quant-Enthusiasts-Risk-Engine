#ifndef JUMPDIFFUSION_H
#define JUMPDIFFUSION_H

#include "../Instrument.h"

namespace JumpDiffusion {
    double mertonOptionPrice(
        double S, double K, double r, double T, double sigma,
        OptionType type, double lambda, double jump_mean, double jump_vol,
        int max_jumps = 50
    );
    
    double mertonCallPrice(
        double S, double K, double r, double T, double sigma,
        double lambda, double jump_mean, double jump_vol,
        int max_jumps = 50
    );
    
    double mertonPutPrice(
        double S, double K, double r, double T, double sigma,
        double lambda, double jump_mean, double jump_vol,
        int max_jumps = 50
    );
    
    double poissonProbability(int n, double lambda_t);
}

#endif