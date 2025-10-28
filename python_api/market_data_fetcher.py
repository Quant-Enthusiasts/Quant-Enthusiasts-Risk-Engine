import yfinance as yf
import sqlite3
import os
from datetime import datetime, timedelta
from typing import Dict, List, Optional, Tuple
import logging

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class MarketDataCache:
    """SQLite-based cache for market data with expiration"""
    
    def __init__(self, db_path: str = "market_data_cache.db"):
        self.db_path = db_path
        self._init_database()
    
    def _init_database(self):
        """Initialize SQLite database with market data table"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS market_data (
                asset_id TEXT PRIMARY KEY,
                spot_price REAL NOT NULL,
                volatility REAL NOT NULL,
                risk_free_rate REAL NOT NULL,
                dividend_yield REAL DEFAULT 0.0,
                last_updated TIMESTAMP NOT NULL,
                data_source TEXT DEFAULT 'yfinance'
            )
        """)
        
        conn.commit()
        conn.close()
        logger.info(f"Market data cache initialized at {self.db_path}")
    
    def get(self, asset_id: str, max_age_hours: int = 24) -> Optional[Dict]:
        """
        Retrieve cached market data if not expired
        
        Args:
            asset_id: Ticker symbol
            max_age_hours: Maximum age in hours before data is considered stale
            
        Returns:
            Dictionary with market data or None if not found/expired
        """
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute("""
            SELECT asset_id, spot_price, volatility, risk_free_rate, 
                   dividend_yield, last_updated, data_source
            FROM market_data
            WHERE asset_id = ?
        """, (asset_id,))
        
        row = cursor.fetchone()
        conn.close()
        
        if not row:
            return None
        
        # Check if data is expired
        last_updated = datetime.fromisoformat(row[5])
        age = datetime.now() - last_updated
        
        if age > timedelta(hours=max_age_hours):
            logger.info(f"Cached data for {asset_id} is expired (age: {age})")
            return None
        
        return {
            'asset_id': row[0],
            'spot': row[1],
            'vol': row[2],
            'rate': row[3],
            'dividend': row[4],
            'last_updated': row[5],
            'source': row[6]
        }
    
    def set(self, asset_id: str, spot: float, vol: float, rate: float, 
            dividend: float = 0.0, source: str = 'yfinance'):
        """
        Store or update market data in cache
        
        Args:
            asset_id: Ticker symbol
            spot: Current spot price
            vol: Volatility (annualized)
            rate: Risk-free rate
            dividend: Dividend yield
            source: Data source identifier
        """
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute("""
            INSERT OR REPLACE INTO market_data 
            (asset_id, spot_price, volatility, risk_free_rate, 
             dividend_yield, last_updated, data_source)
            VALUES (?, ?, ?, ?, ?, ?, ?)
        """, (asset_id, spot, vol, rate, dividend, 
              datetime.now().isoformat(), source))
        
        conn.commit()
        conn.close()
        logger.info(f"Cached market data for {asset_id}")
    
    def get_all(self) -> Dict[str, Dict]:
        """Retrieve all cached market data"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute("""
            SELECT asset_id, spot_price, volatility, risk_free_rate,
                   dividend_yield, last_updated, data_source
            FROM market_data
        """)
        
        rows = cursor.fetchall()
        conn.close()
        
        result = {}
        for row in rows:
            result[row[0]] = {
                'spot': row[1],
                'vol': row[2],
                'rate': row[3],
                'dividend': row[4],
                'last_updated': row[5],
                'source': row[6]
            }
        
        return result
    
    def delete(self, asset_id: str) -> bool:
        """Delete market data for an asset"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute("DELETE FROM market_data WHERE asset_id = ?", (asset_id,))
        deleted = cursor.rowcount > 0
        
        conn.commit()
        conn.close()
        
        return deleted
    
    def clear(self):
        """Clear all cached data"""
        conn = sqlite3.connect(self.db_path)
        cursor = conn.cursor()
        
        cursor.execute("DELETE FROM market_data")
        
        conn.commit()
        conn.close()
        logger.info("Cleared all cached market data")


class MarketDataFetcher:
    """Fetches live market data using YFinance"""
    
    # Default risk-free rate (US 10-year Treasury approximate)
    DEFAULT_RISK_FREE_RATE = 0.045
    
    # Minimum historical data window for volatility calculation
    VOLATILITY_WINDOW_DAYS = 252  # 1 year of trading days
    
    def __init__(self, cache: Optional[MarketDataCache] = None):
        self.cache = cache or MarketDataCache()
    
    def fetch_single(self, ticker: str, force_refresh: bool = False) -> Dict:
        """
        Fetch market data for a single ticker
        
        Args:
            ticker: Stock ticker symbol
            force_refresh: If True, bypass cache and fetch fresh data
            
        Returns:
            Dictionary with market data
            
        Raises:
            ValueError: If ticker is invalid or data unavailable
        """
        ticker = ticker.upper().strip()
        
        if not ticker:
            raise ValueError("Ticker symbol cannot be empty")
        
        # Check cache first unless force refresh
        if not force_refresh:
            cached_data = self.cache.get(ticker)
            if cached_data:
                logger.info(f"Using cached data for {ticker}")
                return cached_data
        
        try:
            # Fetch data from YFinance
            stock = yf.Ticker(ticker)
            
            # Get current price
            try:
                info = stock.info
                spot_price = info.get('currentPrice') or info.get('regularMarketPrice')
                
                if not spot_price:
                    # Fallback to history
                    hist = stock.history(period='1d')
                    if hist.empty:
                        raise ValueError(f"No price data available for {ticker}")
                    spot_price = hist['Close'].iloc[-1]
            except Exception as e:
                raise ValueError(f"Failed to fetch spot price for {ticker}: {str(e)}")
            
            # Get dividend yield
            dividend_yield = info.get('dividendYield', 0.0) or 0.0
            
            # Calculate historical volatility
            volatility = self._calculate_volatility(stock)
            
            # Get or estimate risk-free rate
            risk_free_rate = self._get_risk_free_rate()
            
            # Prepare result
            result = {
                'asset_id': ticker,
                'spot': float(spot_price),
                'vol': float(volatility),
                'rate': float(risk_free_rate),
                'dividend': float(dividend_yield),
                'last_updated': datetime.now().isoformat(),
                'source': 'yfinance'
            }
            
            # Cache the result
            self.cache.set(
                ticker, 
                result['spot'], 
                result['vol'], 
                result['rate'], 
                result['dividend']
            )
            
            logger.info(f"Successfully fetched data for {ticker}")
            return result
            
        except Exception as e:
            logger.error(f"Error fetching data for {ticker}: {str(e)}")
            raise ValueError(f"Failed to fetch data for {ticker}: {str(e)}")
    
    def fetch_multiple(self, tickers: List[str], 
                      force_refresh: bool = False) -> Tuple[Dict, List]:
        """
        Fetch market data for multiple tickers
        
        Args:
            tickers: List of ticker symbols
            force_refresh: If True, bypass cache
            
        Returns:
            Tuple of (successful_data_dict, failed_tickers_list)
        """
        successful = {}
        failed = []
        
        for ticker in tickers:
            try:
                data = self.fetch_single(ticker, force_refresh)
                successful[ticker] = data
            except Exception as e:
                logger.warning(f"Failed to fetch {ticker}: {str(e)}")
                failed.append({'ticker': ticker, 'error': str(e)})
        
        return successful, failed
    
    def _calculate_volatility(self, stock: yf.Ticker, 
                             window_days: int = 252) -> float:
        """
        Calculate annualized historical volatility
        
        Args:
            stock: YFinance Ticker object
            window_days: Historical window in trading days
            
        Returns:
            Annualized volatility
        """
        try:
            # Fetch historical data
            hist = stock.history(period='1y')
            
            if hist.empty or len(hist) < 30:
                logger.warning("Insufficient historical data, using default volatility")
                return 0.25  # Default 25% volatility
            
            # Calculate log returns
            returns = hist['Close'].pct_change().dropna()
            
            if len(returns) < 2:
                return 0.25
            
            # Annualized volatility (assuming 252 trading days)
            volatility = returns.std() * (252 ** 0.5)
            
            # Sanity check: cap between 1% and 200%
            volatility = max(0.01, min(2.0, volatility))
            
            return volatility
            
        except Exception as e:
            logger.warning(f"Error calculating volatility: {str(e)}, using default")
            return 0.25
    
    def _get_risk_free_rate(self) -> float:
        """
        Get current risk-free rate
        
        For now, returns a default rate. Can be enhanced to fetch from FRED API
        or other sources.
        
        Returns:
            Risk-free rate as decimal (e.g., 0.045 for 4.5%)
        """
        try:
            # Attempt to fetch 10-year Treasury rate from YFinance
            treasury = yf.Ticker("^TNX")
            hist = treasury.history(period='1d')
            
            if not hist.empty:
                # TNX is in percentage points, convert to decimal
                rate = hist['Close'].iloc[-1] / 100.0
                return float(rate)
        except:
            pass
        
        # Fallback to default
        return self.DEFAULT_RISK_FREE_RATE


# Global instance
_market_data_fetcher = None

def get_market_data_fetcher() -> MarketDataFetcher:
    """Get or create global MarketDataFetcher instance"""
    global _market_data_fetcher
    if _market_data_fetcher is None:
        _market_data_fetcher = MarketDataFetcher()
    return _market_data_fetcher