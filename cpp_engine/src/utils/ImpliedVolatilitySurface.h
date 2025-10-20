#ifndef IMPLIEDVOLSURFACE_H
#define IMPLIEDVOLSURFACE_H

#include <vector>
#include <map>
#include <string>

namespace VolatilitySurface {
    struct VolPoint {
        double strike;
        double expiry;
        double implied_vol;
    };
    
    class ImpliedVolSurface {
    public:
        void addPoint(double strike, double expiry, double implied_vol);
        double interpolate(double strike, double expiry) const;
        bool hasData() const;
        size_t size() const;
        void clear();
        
        std::vector<VolPoint> getPoints() const;
        
    private:
        std::vector<VolPoint> points_;
        
        double bilinearInterpolation(
            double strike, double expiry,
            const VolPoint& p1, const VolPoint& p2,
            const VolPoint& p3, const VolPoint& p4
        ) const;
    };
    
    double calculateSkew(const std::vector<VolPoint>& points, double expiry);
    double calculateTermStructure(const std::vector<VolPoint>& points, double strike);
}

#endif