#include "Portfolio.h"
#include <algorithm>
#include <sstream>
#include <climits>

void Portfolio::addInstrument(std::unique_ptr<Instrument> instrument, int quantity)
{
    if (!instrument)
    {
        throw std::invalid_argument("Cannot add null instrument to portfolio");
    }

    try
    {
        std::string asset_id = instrument->getAssetId();
        if (asset_id.empty())
        {
            throw std::invalid_argument("Instrument must have a valid asset ID");
        }
    }
    catch (const std::exception &e)
    {
        throw std::invalid_argument(std::string("Invalid instrument: ") + e.what());
    }

    if (instruments.capacity() == instruments.size() && !instruments.empty())
    {
        instruments.reserve(instruments.size() * 2);
    }

    try
    {
        instruments.emplace_back(std::move(instrument), quantity);
    }
    catch (const std::bad_alloc &e)
    {
        throw std::runtime_error("Failed to allocate memory for new instrument");
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error(std::string("Failed to add instrument: ") + e.what());
    }
}

const std::vector<std::pair<std::unique_ptr<Instrument>, int>> &Portfolio::getInstruments() const
{
    return instruments;
}

size_t Portfolio::size() const
{
    return instruments.size();
}

bool Portfolio::empty() const
{
    return instruments.empty();
}

void Portfolio::clear()
{
    instruments.clear();
    instruments.shrink_to_fit();
}

void Portfolio::reserve(size_t capacity)
{
    try
    {
        instruments.reserve(capacity);
    }
    catch (const std::bad_alloc &e)
    {
        throw std::runtime_error("Failed to reserve portfolio capacity");
    }
}

int Portfolio::getTotalQuantityForAsset(const std::string &asset_id) const
{
    if (asset_id.empty())
    {
        throw std::invalid_argument("Asset ID cannot be empty");
    }

    int total = 0;
    for (const auto &[instr, qty] : instruments)
    {
        if (instr && instr->getAssetId() == asset_id)
        {
            if ((qty > 0 && total > INT_MAX - qty) ||
                (qty < 0 && total < INT_MIN - qty))
            {
                throw std::overflow_error("Quantity overflow for asset " + asset_id);
            }
            total += qty;
        }
    }
    return total;
}

void Portfolio::removeInstrument(size_t index)
{
    validateIndex(index);
    instruments.erase(instruments.begin() + index);
}

void Portfolio::updateQuantity(size_t index, int new_quantity)
{
    validateIndex(index);
    instruments[index].second = new_quantity;
}

void Portfolio::validateIndex(size_t index) const
{
    if (index >= instruments.size())
    {
        std::ostringstream oss;
        oss << "Index " << index << " out of range. Portfolio size: " << instruments.size();
        throw std::out_of_range(oss.str());
    }
}