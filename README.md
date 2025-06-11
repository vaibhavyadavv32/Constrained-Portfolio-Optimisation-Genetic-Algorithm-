# Constrained-Portfolio-Optimisation

A C++ implementation of portfolio optimisation using historical stock data, subject to user-defined constraints, with support for maximising Sharpe Ratio or meeting a target return.

## 📁 Project Structure

```
Constrained Portfolio Optimisation/
│
├── Data/
│   ├── Stocks.csv                             # Raw historical stock prices
│   └── Daily Returns.csv                      # Log daily returns of stocks
│
├── Results/
│   ├── Efficient Frontier.csv                 # Efficient frontier data points
│   ├── Efficient Frontier Plot.html           # Interactive visualisation (Plotly)
│   ├── Efficient Frontier Plot.png            # Static image of the plot
│   ├── Optimised Weights - User Target.txt    # Weights for user-defined return
│   ├── Optimised Portfolio - User Target.csv  # Portfolio stats for user target
│   ├── Optimised Weights - Max Sharpe.txt     # Weights for max Sharpe ratio
│   └── Optimised Portfolio - Max Sharpe.csv   # Portfolio stats for max Sharpe
│
├── Scripts/
│   ├── Download Historical Data.py            # Fetch stock price data via API
│   └── Visualise Frontier.py                  # Generate efficient frontier plot
│
├── Include/                                   # C++ header and source files
│   ├── Constraints.hpp                        # Portfolio weight constraints
│   ├── Daily Returns.hpp                      # Returns preprocessing utilities
|   ├── Daily Returns.cpp
│   ├── Matrix.hpp                             # Lightweight matrix operations
│   ├── Matrix.cpp    
│   ├── Optimiser.hpp                          # Genetic algorithm declarations
│   ├── Optimiser.cpp    
│   ├── Portfolio.hpp                          # Portfolio return/risk calculations
│   ├── Portfolio.cpp  
│   └── Utils.hpp                              # Misc utility functions
│
├── main.cpp                                   # Entry point for optimisation
└── README.md                                  # Project documentation
```

## 🛠️ Features

This project supports constrained portfolio optimisation, handling both:
  - Target-return portfolio construction.
  - Maximum Sharpe ratio portfolio selection.

It boasts:
  - Efficient log-return computation from raw prices.
  - Uses a genetic algorithm for robust non-convex optimisation.
  - CSV/HTML result exports with comprehensive plotting via Python.

## ⚙️ Build Instructions
Follow these steps to set up and run the portfolio optimization:
1. **Install Python dependencies** (if required):
   ```bash
   pip install pandas numpy plotly yfinance
   ```
2. **Download Historical Stock Data**:
Open Scripts/Download Historical Data.py. Edit the tickers list to include desired stock symbols (e.g., ["AAPL", "MSFT", "GOOGL"]). Run the script:
```bash
python3 Scripts/Download\ Historical\ Data.py
```
This creates Data/Stocks.csv with historical prices.
3. **Build and Run the C++ Optimizer**
Navigate to the project root directory. Build with g++ (ensure C++17 compatibility):
```bash
g++ -std=c++17 -O2 -I Include main.cpp Include/*.cpp -o optimiser
```
Run the Optimiser:
```bash
optimiser
```
You will be prompted to input a target return. This runs the optimisation and creates results in the Results/ directory.

4. **Visualize the Efficient Frontier**:
Once results are generated, run:
```bash
python3 Scripts/Visualise\ Frontier.py
```
This generates:
  - Results/Efficient Frontier Plot.html (interactive) which displays the plot

## 📊 Inputs & Outputs
**Inputs**
  - Data/Stocks.csv: Historical prices (from Python script)
  - Include/Constraints.hpp: Constraints on weights
  - CLI input: Desired target return

**Outputs**
  - Efficient Frontier.csv: Risk-return frontier points
  - Efficient Frontier Plot.html: Visualisations
  - Optimised Weights - *.txt: Best asset weights
  - Optimised Portfolio - *.csv: Portfolio statistics

## 🔍 How it Works
  - **Preprocessing**: Daily Returns.cpp calculates daily log returns.
  - **Evaluation**: Portfolio.cpp computes return, volatility, Sharpe ratio.
  - **Optimisation**: Optimiser.cpp runs a genetic algorithm with constraints.
  - **Export**: main.cpp writes results to text and CSV files.
  - **Visualisation**: Visualise Frontier.py creates interactive and static plots.

## 📚 Dependencies

  - C++: Standard Library only (C++17)
  - Python:
    - pandas
    - numpy
    - plotly
    - yfinance
---

© 2025 | Developed by Vaibhav Yadav, B.Tech(IIT Madras)
