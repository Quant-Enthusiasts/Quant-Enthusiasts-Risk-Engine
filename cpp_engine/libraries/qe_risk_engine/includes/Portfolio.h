#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include "Instrument.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>

class Portfolio {
public:
    void addInstrument(std::unique_ptr<Instrument> instrument, int quantity);
    
    const std::vector<std::pair<std::unique_ptr<Instrument>, int>>& getInstruments() const;
    
    size_t size() const;
    bool empty() const;
    void clear();
    void reserve(size_t capacity);
    
    int getTotalQuantityForAsset(const std::string& asset_id) const;
    
    void removeInstrument(size_t index);
    
    void updateQuantity(size_t index, int new_quantity);
    
private:
    std::vector<std::pair<std::unique_ptr<Instrument>, int>> instruments;
    
    void validateIndex(size_t index) const;
};

#endif