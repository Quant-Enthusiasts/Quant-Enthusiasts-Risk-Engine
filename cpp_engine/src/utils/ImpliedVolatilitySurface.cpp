#include "ImpliedVolatilitySurface.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <limits>

namespace VolatilitySurface {

void ImpliedVolSurface::addPoint(double strike, double expiry, double implied_vol) {
    if (strike <= 0.0) {
        throw std::invalid_argument("Strike must be positive");
    }
    if (expiry <= 0.0) {
        throw std::invalid_argument("Expiry must be positive");
    }
    if (implied_vol < 0.0 || implied_vol > 10.0) {
        throw std::invalid_argument("Implied volatility out of reasonable range");
    }
    
    points_.push_back({strike, expiry, implied_vol});
}

bool ImpliedVolSurface::hasData() const {
    return !points_.empty();
}

size_t ImpliedVolSurface::size() const {
    return points_.size();
}

void ImpliedVolSurface::clear() {
    points_.clear();
}

std::vector<VolPoint> ImpliedVolSurface::getPoints() const {
    return points_;
}

double ImpliedVolSurface::interpolate(double strike, double expiry) const {
    if (points_.empty()) {
        throw std::runtime_error("No volatility data available");
    }
    
    if (points_.size() == 1) {
        return points_[0].implied_vol;
    }
    
    double min_dist = std::numeric_limits<double>::max();
    double nearest_vol = points_[0].implied_vol;
    
    for (const auto& point : points_) {
        const double dist = std::sqrt(
            std::pow(strike - point.strike, 2) + 
            std::pow(expiry - point.expiry, 2)
        );
        
        if (dist < min_dist) {
            min_dist = dist;
            nearest_vol = point.implied_vol;
        }
    }
    
    return nearest_vol;
}

double calculateSkew(const std::vector<VolPoint>& points, double expiry) {
    std::vector<VolPoint> matching_expiry;
    
    const double tolerance = 0.01;
    for (const auto& point : points) {
        if (std::abs(point.expiry - expiry) < tolerance) {
            matching_expiry.push_back(point);
        }
    }
    
    if (matching_expiry.size() < 2) {
        return 0.0;
    }
    
    std::sort(matching_expiry.begin(), matching_expiry.end(),
              [](const VolPoint& a, const VolPoint& b) {
                  return a.strike < b.strike;
              });
    
    const double low_strike_vol = matching_expiry.front().implied_vol;
    const double high_strike_vol = matching_expiry.back().implied_vol;
    const double strike_range = matching_expiry.back().strike - matching_expiry.front().strike;
    
    if (strike_range < 1e-10) {
        return 0.0;
    }
    
    return (high_strike_vol - low_strike_vol) / strike_range;
}

double calculateTermStructure(const std::vector<VolPoint>& points, double strike) {
    std::vector<VolPoint> matching_strike;
    
    const double tolerance_pct = 0.01;
    for (const auto& point : points) {
        if (std::abs(point.strike - strike) / strike < tolerance_pct) {
            matching_strike.push_back(point);
        }
    }
    
    if (matching_strike.size() < 2) {
        return 0.0;
    }
    
    std::sort(matching_strike.begin(), matching_strike.end(),
              [](const VolPoint& a, const VolPoint& b) {
                  return a.expiry < b.expiry;
              });
    
    const double short_term_vol = matching_strike.front().implied_vol;
    const double long_term_vol = matching_strike.back().implied_vol;
    const double time_range = matching_strike.back().expiry - matching_strike.front().expiry;
    
    if (time_range < 1e-10) {
        return 0.0;
    }
    
    return (long_term_vol - short_term_vol) / time_range;
}

}