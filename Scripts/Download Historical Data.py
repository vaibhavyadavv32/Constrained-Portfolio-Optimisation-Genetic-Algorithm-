# Download Historical Data.py

import yfinance as yf
import pandas as pd
import os

# Define stock tickers grouped by sectors
tickers = [
    # Technology
    "AAPL", "MSFT", "GOOGL", "AMZN", "NVDA", "META", "TSLA", "ADBE", "CRM", "INTC",
    "CSCO", "AMD", "QCOM", "TXN", "AVGO", "ORCL", "IBM", "ACN", "CMCSA", "NFLX",
    "PYPL", "V", "MA", "ADSK", "NOW", "PANW", "SNOW", "CRWD", "DDOG", "ZS",
    # Consumer Discretionary
    "HD", "LOW", "NKE", "MCD", "SBUX", "BKNG", "MAR", "DPZ", "CMG", "LULU",
    # Consumer Staples
    "PG", "KO", "PEP", "COST", "WMT", "UL", "CL", "KHC", "MDLZ", "GIS",
    # Healthcare
    "JNJ", "PFE", "UNH", "LLY", "MRK", "ABBV", "AMGN", "TMO", "DHR", "ISRG",
    "GILD", "VRTX", "BDX", "SYK", "CVS", "MO",
    # Financials
    "JPM", "BAC", "WFC", "SPG", "BLK", "GS", "MS", "CME", "ICE",
    # Industrials
    "GE", "CAT", "MMM", "RTX", "BA", "LMT", "HON", "UPS", "FDX", "DE",
    # Communication Services
    "VZ", "T", "DIS", "CMCSA", "TMUS", "CHTR",
    # Energy & Utilities
    "XOM", "CVX", "NEE", "DUK", "SO", "AEP", "SRE", "XEL",
    # Materials
    "LIN", "APD", "ECL", "SHW", "DD",
    # Real Estate
    "PLD", "EQIX", "AMT", "PSA"
]

# Define date range for historical data
start_date = "2011-01-01"
end_date = "2025-05-31"

# Define output path
output_directory = "Data"
output_filename = "stocks.csv"
output_path = os.path.join(output_directory, output_filename)

try:
    # Download adjusted close prices and forward fill missing values
    data = yf.download(tickers, start=start_date, end=end_date)["Close"]
    data.ffill(inplace=True)

    # Create directory and save data
    os.makedirs(output_directory, exist_ok=True)
    data.to_csv(output_path)

    print(f"Downloaded and saved close prices to {output_path}")
except Exception as e:
    print(f"Error downloading data: {e}")
