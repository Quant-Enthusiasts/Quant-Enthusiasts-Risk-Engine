from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field
from typing import List, Dict, Literal
import quant_risk_engine

app = FastAPI()


class PortfolioItem(BaseModel):
    type: Literal['call', 'put']
    strike: float
    expiry: float
    asset_id: str
    quantity: int


class MarketDataItem(BaseModel):
    spot: float
    rate: float
    vol: float = Field(..., alias='vol')


class CalculateRequest(BaseModel):
    portfolio: List[PortfolioItem]
    market_data: Dict[str, MarketDataItem]


class RiskResult(BaseModel):
    total_pv: float
    total_delta: float
    total_gamma: float
    total_vega: float
    total_theta: float
    value_at_risk_95: float


@app.post('/calculate_risk', response_model=RiskResult)
def calculate_risk(req: CalculateRequest):
    try:
        portfolio_data = req.portfolio
        market_data_map_py = req.market_data

        portfolio = quant_risk_engine.Portfolio()
        for item in portfolio_data:
            option_type = quant_risk_engine.OptionType.Call if item.type.lower() == 'call' else quant_risk_engine.OptionType.Put
            option = quant_risk_engine.EuropeanOption(
                option_type,
                item.strike,
                item.expiry,
                item.asset_id
            )
            portfolio.add_instrument(option, item.quantity)

        market_data_map_cpp = {}
        for asset_id, md_py in market_data_map_py.items():
            md_cpp = quant_risk_engine.MarketData()
            md_cpp.asset_id = asset_id
            md_cpp.spot_price = md_py.spot
            md_cpp.risk_free_rate = md_py.rate
            md_cpp.volatility = md_py.vol
            market_data_map_cpp[asset_id] = md_cpp

        engine = quant_risk_engine.RiskEngine()
        result_cpp = engine.calculate_portfolio_risk(portfolio, market_data_map_cpp)

        result_py = RiskResult(
            total_pv=result_cpp.total_pv,
            total_delta=result_cpp.total_delta,
            total_gamma=result_cpp.total_gamma,
            total_vega=result_cpp.total_vega,
            total_theta=result_cpp.total_theta,
            value_at_risk_95=result_cpp.value_at_risk_95,
        )
        return result_py

    except Exception as e:
        # Map to HTTP 500 with message
        raise HTTPException(status_code=500, detail=str(e))


if __name__ == '__main__':
    # Note: uvicorn should be used to run the app in production.
    import uvicorn

    uvicorn.run('app:app', host='0.0.0.0', port=5000, reload=True)
