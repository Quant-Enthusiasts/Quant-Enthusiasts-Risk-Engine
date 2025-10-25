#include "Instrument.h"
#include "MarketData.h"
#include "Portfolio.h"
#include "simple_test.h"
#include <memory>


void test_empty_portfolio(TestSuite &suite) {
  suite.run_test("Empty portfolio has no instruments", [&]() {
    Portfolio portfolio;

    const auto &instruments = portfolio.getInstruments();

    if (instruments.size() != 0) {
      throw std::runtime_error("Empty portfolio should have size 0");
    }
  });
}

void test_add_single_instrument(TestSuite &suite) {
  suite.run_test("Add single call option", [&]() {
    Portfolio portfolio;

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
        10);

    const auto &instruments = portfolio.getInstruments();

    suite.assert_equal(1, static_cast<double>(instruments.size()), 1e-10,
                       "Portfolio size");
    suite.assert_equal(10, static_cast<double>(instruments[0].second), 1e-10,
                       "Quantity");

    if (instruments[0].first->getAssetId() != "AAPL") {
      throw std::runtime_error("Asset ID mismatch");
    }
  });

  suite.run_test("Add single put option", [&]() {
    Portfolio portfolio;

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Put, 150.0, 0.5, "GOOGL"),
        5);

    const auto &instruments = portfolio.getInstruments();

    suite.assert_equal(1, static_cast<double>(instruments.size()), 1e-10,
                       "Portfolio size");
    suite.assert_equal(5, static_cast<double>(instruments[0].second), 1e-10,
                       "Quantity");
  });
}

void test_add_multiple_instruments(TestSuite &suite) {
  suite.run_test("Add multiple different instruments", [&]() {
    Portfolio portfolio;

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
        10);

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Put, 100.0, 1.0, "AAPL"),
        5);

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Call, 150.0, 0.5, "GOOGL"),
        3);

    const auto &instruments = portfolio.getInstruments();

    suite.assert_equal(3, static_cast<double>(instruments.size()), 1e-10,
                       "Portfolio size");
    suite.assert_equal(10, static_cast<double>(instruments[0].second), 1e-10,
                       "First quantity");
    suite.assert_equal(5, static_cast<double>(instruments[1].second), 1e-10,
                       "Second quantity");
    suite.assert_equal(3, static_cast<double>(instruments[2].second), 1e-10,
                       "Third quantity");
  });
}

void test_quantity_variations(TestSuite &suite) {
  suite.run_test("Positive quantity (long position)", [&]() {
    Portfolio portfolio;

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
        100);

    const auto &instruments = portfolio.getInstruments();
    suite.assert_equal(100, static_cast<double>(instruments[0].second), 1e-10);
  });

  suite.run_test("Negative quantity (short position)", [&]() {
    Portfolio portfolio;

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
        -50);

    const auto &instruments = portfolio.getInstruments();
    suite.assert_equal(-50, static_cast<double>(instruments[0].second), 1e-10);
  });

  suite.run_test("Zero quantity", [&]() {
    Portfolio portfolio;

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
        0);

    const auto &instruments = portfolio.getInstruments();
    suite.assert_equal(1, static_cast<double>(instruments.size()), 1e-10,
                       "Should still add");
    suite.assert_equal(0, static_cast<double>(instruments[0].second), 1e-10,
                       "Quantity is 0");
  });

  suite.run_test("Mixed long and short positions", [&]() {
    Portfolio portfolio;

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
        10);

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Put, 100.0, 1.0, "AAPL"),
        -5);

    const auto &instruments = portfolio.getInstruments();
    suite.assert_equal(2, static_cast<double>(instruments.size()), 1e-10);
    suite.assert_equal(10, static_cast<double>(instruments[0].second), 1e-10,
                       "Long call");
    suite.assert_equal(-5, static_cast<double>(instruments[1].second), 1e-10,
                       "Short put");
  });
}

void test_multiple_assets(TestSuite &suite) {
  suite.run_test("Portfolio with multiple underlying assets", [&]() {
    Portfolio portfolio;

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
        10);

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Call, 150.0, 1.0, "GOOGL"),
        5);

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Put, 200.0, 0.5, "MSFT"),
        3);

    const auto &instruments = portfolio.getInstruments();

    suite.assert_equal(3, static_cast<double>(instruments.size()), 1e-10);

    if (instruments[0].first->getAssetId() != "AAPL") {
      throw std::runtime_error("First asset should be AAPL");
    }
    if (instruments[1].first->getAssetId() != "GOOGL") {
      throw std::runtime_error("Second asset should be GOOGL");
    }
    if (instruments[2].first->getAssetId() != "MSFT") {
      throw std::runtime_error("Third asset should be MSFT");
    }
  });
}

void test_same_asset_multiple_instruments(TestSuite &suite) {
  suite.run_test("Multiple options on same underlying", [&]() {
    Portfolio portfolio;

    // Multiple calls with different strikes on AAPL
    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Call, 95.0, 1.0, "AAPL"),
        10);

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
        5);

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Call, 105.0, 1.0, "AAPL"),
        3);

    const auto &instruments = portfolio.getInstruments();

    suite.assert_equal(3, static_cast<double>(instruments.size()), 1e-10);

    // All should be AAPL
    for (size_t i = 0; i < instruments.size(); ++i) {
      if (instruments[i].first->getAssetId() != "AAPL") {
        throw std::runtime_error("All assets should be AAPL");
      }
    }
  });
}

void test_instrument_ownership(TestSuite &suite) {
  suite.run_test("Portfolio takes ownership of instruments", [&]() {
    Portfolio portfolio;

    // Create instrument outside portfolio scope
    auto option =
        std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL");

    // Portfolio should take ownership
    portfolio.addInstrument(std::move(option), 10);

    // Original unique_ptr should be null after move
    if (option != nullptr) {
      throw std::runtime_error("unique_ptr should be null after move");
    }

    const auto &instruments = portfolio.getInstruments();
    suite.assert_equal(1, static_cast<double>(instruments.size()), 1e-10);
  });
}

void test_large_portfolio(TestSuite &suite) {
  suite.run_test("Portfolio with many instruments", [&]() {
    Portfolio portfolio;

    // Add 100 instruments
    for (int i = 0; i < 100; ++i) {
      portfolio.addInstrument(std::make_unique<EuropeanOption>(
                                  OptionType::Call, 100.0 + i, 1.0, "AAPL"),
                              i + 1);
    }

    const auto &instruments = portfolio.getInstruments();
    suite.assert_equal(100, static_cast<double>(instruments.size()), 1e-10);

    // Verify quantities are correct
    for (int i = 0; i < 100; ++i) {
      suite.assert_equal(i + 1, static_cast<double>(instruments[i].second),
                         1e-10);
    }
  });
}

void test_instrument_pricing_in_portfolio(TestSuite &suite) {
  suite.run_test("Instruments in portfolio can be priced", [&]() {
    Portfolio portfolio;

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "AAPL"),
        1);

    MarketData md;
    md.asset_id = "AAPL";
    md.spot_price = 100.0;
    md.risk_free_rate = 0.05;
    md.volatility = 0.2;

    const auto &instruments = portfolio.getInstruments();
    double price = instruments[0].first->price(md);

    // Should be able to price the instrument
    suite.assert_equal(10.4506, price, 0.01, "Option price");
  });
}

void test_portfolio_ordering(TestSuite &suite) {
  suite.run_test("Instruments maintain insertion order", [&]() {
    Portfolio portfolio;

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "FIRST"),
        1);

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Put, 100.0, 1.0, "SECOND"),
        2);

    portfolio.addInstrument(
        std::make_unique<EuropeanOption>(OptionType::Call, 100.0, 1.0, "THIRD"),
        3);

    const auto &instruments = portfolio.getInstruments();

    if (instruments[0].first->getAssetId() != "FIRST") {
      throw std::runtime_error("First instrument wrong");
    }
    if (instruments[1].first->getAssetId() != "SECOND") {
      throw std::runtime_error("Second instrument wrong");
    }
    if (instruments[2].first->getAssetId() != "THIRD") {
      throw std::runtime_error("Third instrument wrong");
    }
  });
}

int main() {
  TestSuite suite;

  std::cout << "\n" << std::string(60, '=') << std::endl;
  std::cout << "  Portfolio Test Suite" << std::endl;
  std::cout << std::string(60, '=') << "\n" << std::endl;

  test_empty_portfolio(suite);
  test_add_single_instrument(suite);
  test_add_multiple_instruments(suite);
  test_quantity_variations(suite);
  test_multiple_assets(suite);
  test_same_asset_multiple_instruments(suite);
  test_instrument_ownership(suite);
  test_large_portfolio(suite);
  test_instrument_pricing_in_portfolio(suite);
  test_portfolio_ordering(suite);

  suite.print_summary();

  return suite.all_passed() ? 0 : 1;
}
