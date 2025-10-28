from flask import Flask, render_template, request, jsonify, send_from_directory
from flask_cors import CORS
import quant_risk_engine
import traceback
from typing import Dict, Any, Optional, List
from datetime import datetime
from market_data_fetcher import get_market_data_fetcher, MarketDataCache
import os

app = Flask(__name__)
CORS(app)

DEFAULT_VAR_SIMULATIONS = 10000
DEFAULT_VAR_CONFIDENCE = 0.95
DEFAULT_VAR_TIME_HORIZON = 1.0

def validate_portfolio_item(item: Dict[str, Any], index: int) -> None:
    required_fields = ['type', 'strike', 'expiry', 'asset_id', 'quantity']
    for field in required_fields:
        if field not in item:
            raise ValueError(f"Portfolio item {index}: missing required field '{field}'")
    
    option_style = item.get('style', 'european').lower()
    if option_style not in ['european', 'american']:
        raise ValueError(f"Portfolio item {index}: style must be 'european' or 'american'")
    
    if item['type'].lower() not in ['call', 'put']:
        raise ValueError(f"Portfolio item {index}: type must be 'call' or 'put'")
    
    if not isinstance(item['strike'], (int, float)) or item['strike'] <= 0:
        raise ValueError(f"Portfolio item {index}: strike must be a positive number")
    
    if not isinstance(item['expiry'], (int, float)) or item['expiry'] <= 0:
        raise ValueError(f"Portfolio item {index}: expiry must be a positive number")
    
    if not isinstance(item['quantity'], int):
        raise ValueError(f"Portfolio item {index}: quantity must be an integer")
    
    if not isinstance(item['asset_id'], str) or not item['asset_id'].strip():
        raise ValueError(f"Portfolio item {index}: asset_id must be a non-empty string")
    
    pricing_model = item.get('pricing_model', 'blackscholes').lower()
    if pricing_model not in ['blackscholes', 'binomial', 'jumpdiffusion']:
        raise ValueError(f"Portfolio item {index}: pricing_model must be 'blackscholes', 'binomial', or 'jumpdiffusion'")

def validate_market_data(asset_id: str, md: Dict[str, Any]) -> None:
    required_fields = ['spot', 'rate', 'vol']
    for field in required_fields:
        if field not in md:
            raise ValueError(f"Market data for '{asset_id}': missing required field '{field}'")
    
    if not isinstance(md['spot'], (int, float)) or md['spot'] <= 0:
        raise ValueError(f"Market data for '{asset_id}': spot must be a positive number")
    
    if not isinstance(md['rate'], (int, float)):
        raise ValueError(f"Market data for '{asset_id}': rate must be a number")
    
    if not isinstance(md['vol'], (int, float)) or md['vol'] < 0:
        raise ValueError(f"Market data for '{asset_id}': vol must be a non-negative number")
    
    if 'dividend' in md:
        if not isinstance(md['dividend'], (int, float)) or md['dividend'] < 0:
            raise ValueError(f"Market data for '{asset_id}': dividend must be a non-negative number")

def validate_var_parameters(params: Optional[Dict[str, Any]]) -> Dict[str, Any]:
    validated = {
        'simulations': DEFAULT_VAR_SIMULATIONS,
        'confidence': DEFAULT_VAR_CONFIDENCE,
        'time_horizon': DEFAULT_VAR_TIME_HORIZON,
        'seed': None
    }
    
    if params is None:
        return validated
    
    if 'simulations' in params:
        sims = params['simulations']
        if not isinstance(sims, int) or sims <= 0 or sims > 1000000:
            raise ValueError("VaR simulations must be a positive integer <= 1,000,000")
        validated['simulations'] = sims
    
    if 'confidence' in params:
        conf = params['confidence']
        if not isinstance(conf, (int, float)) or conf <= 0.0 or conf >= 1.0:
            raise ValueError("VaR confidence must be between 0 and 1")
        validated['confidence'] = float(conf)
    
    if 'time_horizon' in params:
        horizon = params['time_horizon']
        if not isinstance(horizon, (int, float)) or horizon <= 0.0 or horizon > 252.0:
            raise ValueError("VaR time horizon must be between 0 and 252 days")
        validated['time_horizon'] = float(horizon)
    
    if 'seed' in params:
        seed = params['seed']
        if seed is not None:
            if not isinstance(seed, int) or seed < 0:
                raise ValueError("Random seed must be a non-negative integer")
            validated['seed'] = seed
    
    return validated

def auto_fetch_missing_market_data(portfolio_assets: set, provided_market_data: Dict[str, Any]) -> Dict[str, Any]:
    """
    Automatically fetch market data for assets that aren't provided
    
    Args:
        portfolio_assets: Set of asset IDs in portfolio
        provided_market_data: Market data provided in request
        
    Returns:
        Complete market data dictionary with auto-fetched data
    """
    fetcher = get_market_data_fetcher()
    complete_market_data = {}
    missing_assets = []
    
    for asset_id in portfolio_assets:
        if asset_id in provided_market_data and provided_market_data[asset_id]:
            # Use provided data
            complete_market_data[asset_id] = provided_market_data[asset_id]
        else:
            # Try to fetch from cache or YFinance
            try:
                cached_data = fetcher.cache.get(asset_id)
                if cached_data:
                    logger.info(f"Using cached data for {asset_id}")
                    complete_market_data[asset_id] = {
                        'spot': cached_data['spot'],
                        'rate': cached_data['rate'],
                        'vol': cached_data['vol'],
                        'dividend': cached_data.get('dividend', 0.0)
                    }
                else:
                    # Fetch live data
                    logger.info(f"Fetching live data for {asset_id}")
                    live_data = fetcher.fetch_single(asset_id, force_refresh=False)
                    complete_market_data[asset_id] = {
                        'spot': live_data['spot'],
                        'rate': live_data['rate'],
                        'vol': live_data['vol'],
                        'dividend': live_data.get('dividend', 0.0)
                    }
            except Exception as e:
                logger.error(f"Failed to fetch data for {asset_id}: {str(e)}")
                missing_assets.append(asset_id)
    
    if missing_assets:
        raise ValueError(f"Could not fetch market data for: {', '.join(missing_assets)}. Please provide manually or check ticker symbols.")
    
    return complete_market_data

def create_option(item: Dict[str, Any]) -> Any:
    option_type = (quant_risk_engine.OptionType.Call 
                  if item['type'].lower() == 'call' 
                  else quant_risk_engine.OptionType.Put)
    
    option_style = item.get('style', 'european').lower()
    
    if option_style == 'american':
        binomial_steps = item.get('binomial_steps', 100)
        option = quant_risk_engine.AmericanOption(
            option_type,
            float(item['strike']),
            float(item['expiry']),
            item['asset_id'].strip(),
            binomial_steps
        )
    else:
        pricing_model_str = item.get('pricing_model', 'blackscholes').lower()
        
        if pricing_model_str == 'binomial':
            pricing_model = quant_risk_engine.PricingModel.Binomial
        elif pricing_model_str == 'jumpdiffusion':
            pricing_model = quant_risk_engine.PricingModel.MertonJumpDiffusion
        else:
            pricing_model = quant_risk_engine.PricingModel.BlackScholes
        
        option = quant_risk_engine.EuropeanOption(
            option_type,
            float(item['strike']),
            float(item['expiry']),
            item['asset_id'].strip(),
            pricing_model
        )
        
        if pricing_model_str == 'binomial':
            binomial_steps = item.get('binomial_steps', 100)
            option.set_binomial_steps(binomial_steps)
        
        if pricing_model_str == 'jumpdiffusion':
            jump_params = item.get('jump_parameters', {})
            lambda_val = jump_params.get('lambda', 2.0)
            jump_mean = jump_params.get('mean', -0.05)
            jump_vol = jump_params.get('vol', 0.15)
            option.set_jump_parameters(lambda_val, jump_mean, jump_vol)
    
    return option

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DASHBOARD_DIR = os.path.join(BASE_DIR, "..", "js_dashboard")

@app.route("/")
def serve_dashboard():
    return send_from_directory(DASHBOARD_DIR, "index.html")

@app.route('/update_market_data', methods=['POST'])
def update_market_data():
    """
    Fetch live market data from YFinance and update cache
    """
    try:
        data = request.get_json()
        
        if not data:
            return jsonify({'error': 'Request body must be valid JSON'}), 400
        
        if 'tickers' not in data:
            return jsonify({'error': "Missing required field 'tickers'"}), 400
        
        tickers = data['tickers']
        force_refresh = data.get('force_refresh', False)
        
        if not isinstance(tickers, list):
            return jsonify({'error': "'tickers' must be an array"}), 400
        
        if len(tickers) == 0:
            return jsonify({'error': 'Tickers list cannot be empty'}), 400
        
        if len(tickers) > 50:
            return jsonify({'error': 'Maximum 50 tickers per request'}), 400
        
        for ticker in tickers:
            if not isinstance(ticker, str) or not ticker.strip():
                return jsonify({'error': f'Invalid ticker: {ticker}'}), 400
        
        fetcher = get_market_data_fetcher()
        successful, failed = fetcher.fetch_multiple(tickers, force_refresh)
        
        response = {
            'success': len(failed) == 0,
            'updated': successful,
            'failed': failed,
            'summary': {
                'total_requested': len(tickers),
                'successful': len(successful),
                'failed': len(failed)
            },
            'timestamp': datetime.now().isoformat()
        }
        
        status_code = 200 if len(failed) == 0 else 207
        return jsonify(response), status_code
        
    except ValueError as e:
        return jsonify({'error': f'Validation error: {str(e)}'}), 400
    except Exception as e:
        app.logger.error(f"Unexpected error in update_market_data: {traceback.format_exc()}")
        return jsonify({'error': f'Internal server error: {str(e)}'}), 500


@app.route('/get_cached_market_data', methods=['GET'])
def get_cached_market_data():
    """Retrieve all cached market data"""
    try:
        asset_id = request.args.get('asset_id')
        fetcher = get_market_data_fetcher()
        
        if asset_id:
            cached_data = fetcher.cache.get(asset_id.upper())
            if not cached_data:
                return jsonify({'error': f'No cached data for {asset_id}'}), 404
            return jsonify({asset_id.upper(): cached_data}), 200
        else:
            all_data = fetcher.cache.get_all()
            return jsonify(all_data), 200
            
    except Exception as e:
        app.logger.error(f"Error retrieving cached data: {traceback.format_exc()}")
        return jsonify({'error': f'Internal server error: {str(e)}'}), 500


@app.route('/clear_market_data_cache', methods=['DELETE'])
def clear_market_data_cache():
    """Clear all cached market data"""
    try:
        fetcher = get_market_data_fetcher()
        fetcher.cache.clear()
        
        return jsonify({
            'success': True,
            'message': 'Market data cache cleared',
            'timestamp': datetime.now().isoformat()
        }), 200
        
    except Exception as e:
        app.logger.error(f"Error clearing cache: {traceback.format_exc()}")
        return jsonify({'error': f'Internal server error: {str(e)}'}), 500


@app.route('/health', methods=['GET'])
def health_check():
    try:
        fetcher = get_market_data_fetcher()
        cached_assets = len(fetcher.cache.get_all())
        
        return jsonify({
            'status': 'healthy',
            'service': 'quant-risk-engine',
            'version': '3.1',
            'features': [
                'european_options',
                'american_options',
                'multiple_pricing_models',
                'jump_diffusion',
                'portfolio_analytics',
                'var_calculation',
                'live_market_data',
                'auto_fetch_market_data'  # New feature
            ],
            'cache_info': {
                'cached_assets': cached_assets,
                'cache_location': fetcher.cache.db_path
            }
        }), 200
    except Exception as e:
        return jsonify({
            'status': 'degraded',
            'error': str(e)
        }), 500

@app.route("/documentation")
@app.route("/documentation.html")
def documentation():
    return send_from_directory(DASHBOARD_DIR, "documentation.html")

@app.route('/calculate_risk', methods=['POST'])
def calculate_risk():
    """
    Calculate portfolio risk with automatic market data fetching
    Market data can be:
    1. Provided in request
    2. Empty {} - will auto-fetch from cache/YFinance
    3. Partial - will auto-fetch missing assets
    """
    try:
        data = request.get_json()
        
        if not data:
            return jsonify({'error': 'Request body must be valid JSON'}), 400
        
        if 'portfolio' not in data:
            return jsonify({'error': "Missing required field 'portfolio'"}), 400
        
        portfolio_data = data['portfolio']
        market_data_map_py = data.get('market_data', {})  # Default to empty dict
        var_params = data.get('var_parameters', None)
        
        if not isinstance(portfolio_data, list):
            return jsonify({'error': "Field 'portfolio' must be an array"}), 400
        
        if not isinstance(market_data_map_py, dict):
            return jsonify({'error': "Field 'market_data' must be an object"}), 400
        
        if len(portfolio_data) == 0:
            return jsonify({'error': 'Portfolio cannot be empty'}), 400

        # Validate portfolio items
        for idx, item in enumerate(portfolio_data):
            validate_portfolio_item(item, idx)
        
        # Get all unique assets in portfolio
        portfolio_assets = set(item['asset_id'] for item in portfolio_data)
        
        # AUTO-FETCH: Get complete market data (provided + auto-fetched)
        try:
            complete_market_data = auto_fetch_missing_market_data(
                portfolio_assets, 
                market_data_map_py
            )
            
            # Track which assets were auto-fetched for response
            auto_fetched = [asset for asset in portfolio_assets 
                          if asset not in market_data_map_py or not market_data_map_py[asset]]
            
        except ValueError as e:
            return jsonify({'error': str(e)}), 400
        
        # Validate the complete market data
        for asset_id, md in complete_market_data.items():
            validate_market_data(asset_id, md)
        
        var_config = validate_var_parameters(var_params)

        # Build portfolio
        portfolio = quant_risk_engine.Portfolio()
        portfolio.reserve(len(portfolio_data))
        
        for item in portfolio_data:
            option = create_option(item)
            portfolio.add_instrument(option, item['quantity'])

        # Convert market data to C++ format
        market_data_map_cpp = {}
        for asset_id, md_py in complete_market_data.items():
            dividend = md_py.get('dividend', 0.0)
            md_cpp = quant_risk_engine.MarketData(
                asset_id,
                float(md_py['spot']),
                float(md_py['rate']),
                float(md_py['vol']),
                float(dividend)
            )
            market_data_map_cpp[asset_id] = md_cpp

        # Calculate risk
        engine = quant_risk_engine.RiskEngine()
        engine.set_var_simulations(var_config['simulations'])
        engine.set_var_time_horizon_days(var_config['time_horizon'])
        
        if var_config['seed'] is not None:
            engine.set_random_seed(var_config['seed'])
            engine.set_use_fixed_seed(True)
        
        result_cpp = engine.calculate_portfolio_risk(portfolio, market_data_map_cpp)
        
        if not result_cpp.is_valid():
            return jsonify({'error': 'Risk calculation produced invalid results'}), 500

        # Prepare response with market data info
        result_py = {
            'total_pv': result_cpp.total_pv,
            'total_delta': result_cpp.total_delta,
            'total_gamma': result_cpp.total_gamma,
            'total_vega': result_cpp.total_vega,
            'total_theta': result_cpp.total_theta,
            'value_at_risk_95': result_cpp.value_at_risk_95,
            'portfolio_size': len(portfolio),
            'var_parameters': {
                'simulations': var_config['simulations'],
                'confidence_level': var_config['confidence'],
                'time_horizon_days': var_config['time_horizon']
            },
            'market_data_info': {
                'auto_fetched_assets': auto_fetched if auto_fetched else [],
                'market_data_used': complete_market_data
            }
        }
        return jsonify(result_py), 200

    except ValueError as e:
        return jsonify({'error': f'Validation error: {str(e)}'}), 400
    except RuntimeError as e:
        return jsonify({'error': f'Runtime error: {str(e)}'}), 500
    except Exception as e:
        app.logger.error(f"Unexpected error: {traceback.format_exc()}")
        return jsonify({'error': f'Internal server error: {str(e)}'}), 500

@app.route('/price_option', methods=['POST'])
def price_option():
    """
    Price a single option with automatic market data fetching
    """
    try:
        data = request.get_json()
        
        if not data:
            return jsonify({'error': 'Request body must be valid JSON'}), 400
        
        required_fields = ['type', 'strike', 'expiry', 'asset_id']
        for field in required_fields:
            if field not in data:
                return jsonify({'error': f"Missing required field '{field}'"}), 400

        
        asset_id = data['asset_id']
        market_data_provided = data.get('market_data', {})
        
        # AUTO-FETCH market data if not provided
        if not market_data_provided or not all(k in market_data_provided for k in ['spot', 'rate', 'vol']):
            fetcher = get_market_data_fetcher()
            try:
                # Try cache first
                cached_data = fetcher.cache.get(asset_id)
                if cached_data:
                    md_data = {
                        'spot': market_data_provided.get('spot', cached_data['spot']),
                        'rate': market_data_provided.get('rate', cached_data['rate']),
                        'vol': market_data_provided.get('vol', cached_data['vol']),
                        'dividend': market_data_provided.get('dividend', cached_data.get('dividend', 0.0))
                    }
                    auto_fetched = True
                else:
                    # Fetch live
                    live_data = fetcher.fetch_single(asset_id, force_refresh=False)
                    md_data = {
                        'spot': market_data_provided.get('spot', live_data['spot']),
                        'rate': market_data_provided.get('rate', live_data['rate']),
                        'vol': market_data_provided.get('vol', live_data['vol']),
                        'dividend': market_data_provided.get('dividend', live_data.get('dividend', 0.0))
                    }
                    auto_fetched = True
            except Exception as e:
                return jsonify({'error': f"Could not fetch market data for {asset_id}: {str(e)}. Please provide manually."}), 400
        else:
            md_data = market_data_provided
            auto_fetched = False
        
        validate_market_data(asset_id, md_data)
        
        option = create_option({
            'type': data['type'],
            'strike': data['strike'],
            'expiry': data['expiry'],
            'asset_id': asset_id,
            'style': data.get('style', 'european'),
            'pricing_model': data.get('pricing_model', 'blackscholes'),
            'binomial_steps': data.get('binomial_steps', 100),
            'jump_parameters': data.get('jump_parameters', {}),
            'quantity': 1
        })
        
        md = quant_risk_engine.MarketData(
            asset_id,
            float(md_data['spot']),
            float(md_data['rate']),
            float(md_data['vol']),
            float(md_data.get('dividend', 0.0))
        )
        
        result = {
            'price': option.price(md),
            'delta': option.delta(md),
            'gamma': option.gamma(md),
            'vega': option.vega(md),
            'theta': option.theta(md),
            'instrument_type': option.get_instrument_type(),
            'market_data_auto_fetched': auto_fetched,
            'market_data_used': md_data
        }
        
        return jsonify(result), 200
        
    except ValueError as e:
        return jsonify({'error': f'Validation error: {str(e)}'}), 400
    except RuntimeError as e:
        return jsonify({'error': f'Runtime error: {str(e)}'}), 500
    except Exception as e:
        app.logger.error(f"Unexpected error: {traceback.format_exc()}")
        return jsonify({'error': f'Internal server error: {str(e)}'}), 500

@app.route('/portfolio/net_position/<asset_id>', methods=['POST'])
def get_net_position(asset_id):
    try:
        data = request.get_json()
        
        if not data or 'portfolio' not in data:
            return jsonify({'error': 'Missing portfolio data'}), 400
        
        portfolio_data = data['portfolio']
        
        if not isinstance(portfolio_data, list):
            return jsonify({'error': 'Portfolio must be an array'}), 400
        
        for idx, item in enumerate(portfolio_data):
            validate_portfolio_item(item, idx)
        
        portfolio = quant_risk_engine.Portfolio()
        
        for item in portfolio_data:
            option = create_option(item)
            portfolio.add_instrument(option, item['quantity'])
        
        net_quantity = portfolio.get_total_quantity(asset_id)
        
        return jsonify({
            'asset_id': asset_id,
            'net_quantity': net_quantity,
            'direction': 'long' if net_quantity > 0 else ('short' if net_quantity < 0 else 'flat')
        }), 200
        
    except ValueError as e:
        return jsonify({'error': f'Validation error: {str(e)}'}), 400
    except RuntimeError as e:
        return jsonify({'error': f'Runtime error: {str(e)}'}), 500
    except Exception as e:
        app.logger.error(f"Unexpected error: {traceback.format_exc()}")
        return jsonify({'error': f'Internal server error: {str(e)}'}), 500

@app.route('/portfolio/summary', methods=['POST'])
def portfolio_summary():
    try:
        data = request.get_json()
        
        if not data or 'portfolio' not in data:
            return jsonify({'error': 'Missing portfolio data'}), 400
        
        portfolio_data = data['portfolio']
        
        if not isinstance(portfolio_data, list):
            return jsonify({'error': 'Portfolio must be an array'}), 400
        
        for idx, item in enumerate(portfolio_data):
            validate_portfolio_item(item, idx)
        
        portfolio = quant_risk_engine.Portfolio()
        
        for item in portfolio_data:
            option = create_option(item)
            portfolio.add_instrument(option, item['quantity'])
        
        assets = set(item['asset_id'] for item in portfolio_data)
        net_positions = {}
        for asset in assets:
            net_positions[asset] = portfolio.get_total_quantity(asset)
        
        instrument_counts = {
            'european': sum(1 for item in portfolio_data if item.get('style', 'european') == 'european'),
            'american': sum(1 for item in portfolio_data if item.get('style', 'european') == 'american'),
            'calls': sum(1 for item in portfolio_data if item['type'].lower() == 'call'),
            'puts': sum(1 for item in portfolio_data if item['type'].lower() == 'put')
        }
        
        return jsonify({
            'portfolio_size': len(portfolio),
            'unique_assets': len(assets),
            'net_positions': net_positions,
            'instrument_counts': instrument_counts
        }), 200
        
    except ValueError as e:
        return jsonify({'error': f'Validation error: {str(e)}'}), 400
    except RuntimeError as e:
        return jsonify({'error': f'Runtime error: {str(e)}'}), 500
    except Exception as e:
        app.logger.error(f"Unexpected error: {traceback.format_exc()}")
        return jsonify({'error': f'Internal server error: {str(e)}'}), 500

@app.errorhandler(404)
def not_found(error):
    return jsonify({'error': 'Endpoint not found'}), 404

@app.errorhandler(405)
def method_not_allowed(error):
    return jsonify({'error': 'Method not allowed'}), 405

@app.errorhandler(500)
def internal_error(error):
    return jsonify({'error': 'Internal server error'}), 500

# Add logging import
import logging
logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO)

if __name__ == '__main__':
    port = int(os.environ.get("PORT", 10000)) 
    app.run(debug=True, host="0.0.0.0", port=port)