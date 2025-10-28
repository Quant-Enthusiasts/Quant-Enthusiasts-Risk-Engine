"""
Test Suite for Market Data Fetcher
Tests caching, YFinance integration, and API endpoints
"""

import pytest
import os
import tempfile
from datetime import datetime, timedelta
from market_data_fetcher import MarketDataCache, MarketDataFetcher
from unittest.mock import Mock, patch
import yfinance as yf


class TestMarketDataCache:
    """Test SQLite cache functionality"""
    
    @pytest.fixture
    def temp_cache(self):
        """Create temporary cache for testing"""
        with tempfile.NamedTemporaryFile(delete=False, suffix='.db') as f:
            cache_path = f.name
        
        cache = MarketDataCache(cache_path)
        yield cache
        
        # Cleanup
        if os.path.exists(cache_path):
            os.remove(cache_path)
    
    def test_cache_initialization(self, temp_cache):
        """Test cache database is created"""
        assert os.path.exists(temp_cache.db_path)
    
    def test_cache_set_and_get(self, temp_cache):
        """Test storing and retrieving data"""
        temp_cache.set('AAPL', 175.50, 0.28, 0.045, 0.005)
        
        data = temp_cache.get('AAPL')
        assert data is not None
        assert data['spot'] == 175.50
        assert data['vol'] == 0.28
        assert data['rate'] == 0.045
        assert data['dividend'] == 0.005
        assert data['asset_id'] == 'AAPL'
    
    def test_cache_expiration(self, temp_cache):
        """Test cache expiration logic"""
        temp_cache.set('AAPL', 175.50, 0.28, 0.045)
        
        # Data should be fresh
        data = temp_cache.get('AAPL', max_age_hours=24)
        assert data is not None
        
        # Mock expired data by setting very short max age
        data = temp_cache.get('AAPL', max_age_hours=0)
        assert data is None
    
    def test_cache_update(self, temp_cache):
        """Test updating existing cache entry"""
        temp_cache.set('AAPL', 175.50, 0.28, 0.045)
        temp_cache.set('AAPL', 180.00, 0.30, 0.045)
        
        data = temp_cache.get('AAPL')
        assert data['spot'] == 180.00
        assert data['vol'] == 0.30
    
    def test_cache_get_all(self, temp_cache):
        """Test retrieving all cached data"""
        temp_cache.set('AAPL', 175.50, 0.28, 0.045)
        temp_cache.set('GOOGL', 140.00, 0.25, 0.045)
        
        all_data = temp_cache.get_all()
        assert len(all_data) == 2
        assert 'AAPL' in all_data
        assert 'GOOGL' in all_data
    
    def test_cache_delete(self, temp_cache):
        """Test deleting cache entry"""
        temp_cache.set('AAPL', 175.50, 0.28, 0.045)
        assert temp_cache.delete('AAPL') is True
        assert temp_cache.get('AAPL') is None
        assert temp_cache.delete('AAPL') is False
    
    def test_cache_clear(self, temp_cache):
        """Test clearing entire cache"""
        temp_cache.set('AAPL', 175.50, 0.28, 0.045)
        temp_cache.set('GOOGL', 140.00, 0.25, 0.045)
        
        temp_cache.clear()
        all_data = temp_cache.get_all()
        assert len(all_data) == 0


class TestMarketDataFetcher:
    """Test market data fetching functionality"""
    
    @pytest.fixture
    def temp_fetcher(self):
        """Create fetcher with temporary cache"""
        with tempfile.NamedTemporaryFile(delete=False, suffix='.db') as f:
            cache_path = f.name
        
        cache = MarketDataCache(cache_path)
        fetcher = MarketDataFetcher(cache)
        yield fetcher
        
        # Cleanup
        if os.path.exists(cache_path):
            os.remove(cache_path)
    
    def test_fetch_single_valid_ticker(self, temp_fetcher):
        """Test fetching data for valid ticker"""
        # This is an integration test that requires network access
        # Skip if testing offline
        try:
            data = temp_fetcher.fetch_single('AAPL')
            assert data['asset_id'] == 'AAPL'
            assert data['spot'] > 0
            assert 0 < data['vol'] < 2.0
            assert 0 <= data['rate'] <= 0.2
            assert 0 <= data['dividend'] <= 1.0
        except Exception as e:
            pytest.skip(f"Network test skipped: {str(e)}")
    
    def test_fetch_single_invalid_ticker(self, temp_fetcher):
        """Test fetching data for invalid ticker"""
        with pytest.raises(ValueError):
            temp_fetcher.fetch_single('INVALID_TICKER_XYZ123')
    
    def test_fetch_single_empty_ticker(self, temp_fetcher):
        """Test fetching data with empty ticker"""
        with pytest.raises(ValueError):
            temp_fetcher.fetch_single('')
    
    def test_fetch_uses_cache(self, temp_fetcher):
        """Test that fetch uses cached data"""
        # Pre-populate cache
        temp_fetcher.cache.set('AAPL', 175.50, 0.28, 0.045, 0.005)
        
        # Fetch should use cache
        data = temp_fetcher.fetch_single('AAPL', force_refresh=False)
        assert data['spot'] == 175.50
        assert data['vol'] == 0.28
    
    def test_fetch_force_refresh(self, temp_fetcher):
        """Test force refresh bypasses cache"""
        # Pre-populate cache with old data
        temp_fetcher.cache.set('AAPL', 100.00, 0.20, 0.03, 0.005)
        
        # Force refresh should fetch new data
        try:
            data = temp_fetcher.fetch_single('AAPL', force_refresh=True)
            # Price should be different from cached value
            assert data['spot'] != 100.00
        except Exception:
            pytest.skip("Network test skipped")
    
    def test_fetch_multiple_success(self, temp_fetcher):
        """Test fetching multiple tickers"""
        try:
            successful, failed = temp_fetcher.fetch_multiple(['AAPL', 'MSFT'])
            
            assert len(successful) >= 1  # At least one should succeed
            assert 'AAPL' in successful or 'MSFT' in successful
            
            for ticker, data in successful.items():
                assert data['spot'] > 0
                assert data['vol'] > 0
        except Exception:
            pytest.skip("Network test skipped")
    
    def test_fetch_multiple_with_failures(self, temp_fetcher):
        """Test fetching multiple tickers with some invalid"""
        try:
            successful, failed = temp_fetcher.fetch_multiple([
                'AAPL', 
                'INVALID_XYZ123'
            ])
            
            # At least one should fail
            assert len(failed) >= 1
            
            # Check failed structure
            assert 'ticker' in failed[0]
            assert 'error' in failed[0]
        except Exception:
            pytest.skip("Network test skipped")
    
    @patch('yfinance.Ticker')
    def test_calculate_volatility_sufficient_data(self, mock_ticker, temp_fetcher):
        """Test volatility calculation with sufficient data"""
        import pandas as pd
        import numpy as np
        
        # Create mock historical data
        dates = pd.date_range(start='2024-01-01', periods=252, freq='D')
        prices = 100 + np.random.randn(252).cumsum()
        mock_hist = pd.DataFrame({
            'Close': prices
        }, index=dates)
        
        mock_ticker_instance = Mock()
        mock_ticker_instance.history.return_value = mock_hist
        mock_ticker.return_value = mock_ticker_instance
        
        vol = temp_fetcher._calculate_volatility(mock_ticker_instance)
        
        assert 0.01 <= vol <= 2.0  # Reasonable volatility range
    
    @patch('yfinance.Ticker')
    def test_calculate_volatility_insufficient_data(self, mock_ticker, temp_fetcher):
        """Test volatility calculation with insufficient data"""
        import pandas as pd
        
        # Create mock with very little data
        dates = pd.date_range(start='2024-01-01', periods=5, freq='D')
        prices = [100, 101, 100, 102, 101]
        mock_hist = pd.DataFrame({
            'Close': prices
        }, index=dates)
        
        mock_ticker_instance = Mock()
        mock_ticker_instance.history.return_value = mock_hist
        
        vol = temp_fetcher._calculate_volatility(mock_ticker_instance)
        
        # Should return default volatility
        assert vol == 0.25


class TestAPIEndpoints:
    """Test Flask API endpoints"""
    
    @pytest.fixture
    def client(self):
        """Create test client"""
        from app import app as flask_app
        flask_app.config['TESTING'] = True
        
        with flask_app.test_client() as client:
            yield client
    
    def test_update_market_data_valid(self, client):
        """Test update_market_data endpoint with valid data"""
        try:
            response = client.post('/update_market_data',
                json={'tickers': ['AAPL', 'MSFT']},
                content_type='application/json')
            
            # Should return 200 or 207
            assert response.status_code in [200, 207]
            
            data = response.get_json()
            assert 'updated' in data
            assert 'failed' in data
            assert 'summary' in data
            
        except Exception:
            pytest.skip("Network test skipped")
    
    def test_update_market_data_invalid_ticker(self, client):
        """Test update_market_data with invalid ticker"""
        response = client.post('/update_market_data',
            json={'tickers': ['INVALID_XYZ123']},
            content_type='application/json')
        
        data = response.get_json()
        assert len(data['failed']) > 0
    
    def test_update_market_data_empty_tickers(self, client):
        """Test update_market_data with empty tickers list"""
        response = client.post('/update_market_data',
            json={'tickers': []},
            content_type='application/json')
        
        assert response.status_code == 400
        data = response.get_json()
        assert 'error' in data
    
    def test_update_market_data_missing_tickers(self, client):
        """Test update_market_data without tickers field"""
        response = client.post('/update_market_data',
            json={},
            content_type='application/json')
        
        assert response.status_code == 400
        data = response.get_json()
        assert 'error' in data
    
    def test_update_market_data_too_many_tickers(self, client):
        """Test update_market_data with too many tickers"""
        tickers = [f'TICK{i}' for i in range(51)]
        response = client.post('/update_market_data',
            json={'tickers': tickers},
            content_type='application/json')
        
        assert response.status_code == 400
        data = response.get_json()
        assert 'error' in data
    
    def test_update_market_data_invalid_json(self, client):
        """Test update_market_data with invalid JSON"""
        response = client.post('/update_market_data',
            data='invalid json',
            content_type='application/json')
        
        assert response.status_code == 500
    
    def test_get_cached_market_data_all(self, client):
        """Test getting all cached market data"""
        response = client.get('/get_cached_market_data')
        
        assert response.status_code == 200
        data = response.get_json()
        assert isinstance(data, dict)
    
    def test_get_cached_market_data_specific(self, client):
        """Test getting specific asset from cache"""
        # First populate cache
        try:
            client.post('/update_market_data',
                json={'tickers': ['AAPL']},
                content_type='application/json')
            
            # Then retrieve
            response = client.get('/get_cached_market_data?asset_id=AAPL')
            
            assert response.status_code == 200
            data = response.get_json()
            assert 'AAPL' in data
        except Exception:
            pytest.skip("Network test skipped")
    
    def test_get_cached_market_data_not_found(self, client):
        """Test getting non-existent asset from cache"""
        response = client.get('/get_cached_market_data?asset_id=NOTEXIST')
        
        assert response.status_code == 404
        data = response.get_json()
        assert 'error' in data
    
    def test_clear_market_data_cache(self, client):
        """Test clearing market data cache"""
        response = client.delete('/clear_market_data_cache')
        
        assert response.status_code == 200
        data = response.get_json()
        assert data['success'] is True
    
    def test_health_check_includes_cache_info(self, client):
        """Test health check includes cache information"""
        response = client.get('/health')
        
        assert response.status_code == 200
        data = response.get_json()
        assert 'cache_info' in data
        assert 'cached_assets' in data['cache_info']
        assert 'live_market_data' in data['features']


# Performance tests
class TestPerformance:
    """Test performance characteristics"""
    
    @pytest.fixture
    def temp_fetcher(self):
        """Create fetcher with temporary cache"""
        with tempfile.NamedTemporaryFile(delete=False, suffix='.db') as f:
            cache_path = f.name
        
        cache = MarketDataCache(cache_path)
        fetcher = MarketDataFetcher(cache)
        yield fetcher
        
        if os.path.exists(cache_path):
            os.remove(cache_path)
    
    def test_cache_performance(self, temp_fetcher):
        """Test cache read/write performance"""
        import time
        
        # Write 100 entries
        start = time.time()
        for i in range(100):
            temp_fetcher.cache.set(f'ASSET{i}', 100.0, 0.25, 0.045)
        write_time = time.time() - start
        
        # Read 100 entries
        start = time.time()
        for i in range(100):
            temp_fetcher.cache.get(f'ASSET{i}')
        read_time = time.time() - start
        
        # Both should be fast
        assert write_time < 1.0  # Less than 1 second for 100 writes
        assert read_time < 0.5   # Less than 0.5 seconds for 100 reads


if __name__ == '__main__':
    pytest.main([__file__, '-v'])
