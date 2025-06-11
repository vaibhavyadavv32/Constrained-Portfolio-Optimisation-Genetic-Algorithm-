import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
import os
import numpy as np

# Define file paths
results_dir = "Results"
efficient_frontier_csv = os.path.join(results_dir, "Efficient Frontier.csv")
optimised_user_target_csv = os.path.join(results_dir, "User Portfolio.csv")
optimised_max_sharpe_csv = os.path.join(results_dir, "Best Sharpe Portfolio.csv")
output_html = os.path.join(results_dir, "Efficient Frontier Plot.html")

print("--- Visualizing Efficient Frontier ---")

# Load Efficient Frontier CSV
if not os.path.exists(efficient_frontier_csv):
    print(f"Error: Efficient Frontier data not found at '{efficient_frontier_csv}'.")
    exit()

try:
    df_frontier = pd.read_csv(efficient_frontier_csv)
    if df_frontier.empty:
        print(f"Warning: '{efficient_frontier_csv}' is empty.")
        exit()
    print(f"Loaded {len(df_frontier)} points for the Efficient Frontier.")
except Exception as e:
    print(f"Error loading Efficient Frontier CSV: {e}")
    exit()

# Load portfolio metrics from file
def load_portfolio_details(filepath, portfolio_name):
    ret, risk, sharpe = None, None, None
    if not os.path.exists(filepath):
        print(f"Warning: {portfolio_name} data not found at '{filepath}'.")
        return ret, risk, sharpe

    try:
        df = pd.read_csv(filepath, nrows=3)
        metrics = df.set_index('Metric')['Value'].to_dict()
        ret = metrics.get('Expected Return')
        risk = metrics.get('Portfolio Std Dev')
        sharpe = metrics.get('Sharpe Ratio')

        if all(v is not None for v in [ret, risk, sharpe]):
            print(f"Loaded {portfolio_name}: Return={ret:.6f}, Risk={risk:.6f}, Sharpe={sharpe:.6f}")
        else:
            print(f"Warning: Missing metrics in {portfolio_name}.")
            ret, risk, sharpe = None, None, None
    except Exception as e:
        print(f"Error loading {portfolio_name}: {e}")
        ret, risk, sharpe = None, None, None
    return ret, risk, sharpe

# Load portfolios
user_target_return, user_target_risk, user_target_sharpe = load_portfolio_details(optimised_user_target_csv, "User's Target Portfolio")
optimal_sharpe_return, optimal_sharpe_risk, optimal_sharpe_value = load_portfolio_details(optimised_max_sharpe_csv, "Max Sharpe Portfolio")

# Create base line plot
fig = px.line(
    df_frontier, x="Risk", y="Return",
    title="Efficient Frontier - Genetic Algorithm Optimization",
    labels={"Risk": "Annualized Portfolio Standard Deviation", "Return": "Annualized Expected Portfolio Return"},
    line_shape="spline",
    markers=True
)

# Enhance marker visibility
fig.update_traces(marker=dict(size=8, opacity=0.7, line=dict(width=1, color='DarkSlateGrey')),
                  selector=dict(mode='markers'))

# Add individual frontier points
fig.add_trace(go.Scatter(
    x=df_frontier["Risk"], y=df_frontier["Return"], mode="markers",
    name="Frontier Portfolios", marker=dict(color="blue", size=8, opacity=0.7)
))

# Add horizontal line and marker for user target return
if user_target_return is not None:
    min_risk_plot = df_frontier['Risk'].min() * 0.9
    max_risk_plot = df_frontier['Risk'].max() * 1.1
    fig.add_trace(go.Scatter(
        x=[min_risk_plot, max_risk_plot],
        y=[user_target_return, user_target_return],
        mode="lines", name=f"User Target Return ({user_target_return:.4f})",
        line=dict(dash="dash", color="green", width=2), hoverinfo="skip"
    ))
    if user_target_risk is not None:
        fig.add_trace(go.Scatter(
            x=[user_target_risk], y=[user_target_return], mode="markers+text",
            marker=dict(color="darkgreen", size=14, symbol="circle-open",
                        line=dict(color="darkgreen", width=2)),
            name=f"User's Optimized Portfolio (Sharpe: {user_target_sharpe:.3f})",
            text=[f"User Target Achieved<br>Risk: {user_target_risk:.4f}<br>Return: {user_target_return:.4f}<br>Sharpe: {user_target_sharpe:.3f}"],
            textposition="top center", hoverinfo="text+x+y"
        ))
        print(f"Highlighted User's Target Portfolio at (Risk: {user_target_risk:.6f}, Return: {user_target_return:.6f}).")

# Highlight max Sharpe portfolio
if optimal_sharpe_risk is not None and optimal_sharpe_return is not None:
    fig.add_trace(go.Scatter(
        x=[optimal_sharpe_risk], y=[optimal_sharpe_return], mode="markers+text",
        marker=dict(color="red", size=12, symbol="star", line=dict(color="black", width=2)),
        name=f"Max Sharpe Portfolio (Sharpe: {optimal_sharpe_value:.3f})",
        text=[f"Max Sharpe Portfolio<br>Risk: {optimal_sharpe_risk:.4f}<br>Return: {optimal_sharpe_return:.4f}<br>Sharpe: {optimal_sharpe_value:.3f}"],
        textposition="top center", hoverinfo="text+x+y"
    ))
    print(f"Highlighted Max Sharpe Portfolio at (Risk: {optimal_sharpe_risk:.6f}, Return: {optimal_sharpe_return:.6f}).")

# Highlight min risk portfolio
if not df_frontier.empty:
    min_risk_row = df_frontier.loc[df_frontier['Risk'].idxmin()]
    fig.add_trace(go.Scatter(
        x=[min_risk_row["Risk"]], y=[min_risk_row["Return"]], mode="markers+text",
        marker=dict(color="purple", size=12, symbol="diamond", line=dict(color="black", width=2)),
        name="Min Risk Portfolio",
        text=[f"Min Risk Portfolio<br>Risk: {min_risk_row['Risk']:.4f}<br>Return: {min_risk_row['Return']:.4f}"],
        textposition="bottom center", hoverinfo="text+x+y"
    ))
    print(f"Highlighted Minimum Risk Portfolio at (Risk: {min_risk_row['Risk']:.6f}, Return: {min_risk_row['Return']:.6f}).")

# Plot layout
fig.update_layout(
    title_font_size=24, title_x=0.5,
    xaxis_title_font_size=16, yaxis_title_font_size=16,
    hovermode="closest",
    legend=dict(x=0.01, y=0.99, xanchor="left", yanchor="top",
                bgcolor="rgba(255,255,255,0.8)", bordercolor="Black", borderwidth=1),
    template="plotly_white",
    xaxis=dict(gridcolor='lightgrey', zeroline=True, zerolinewidth=2, zerolinecolor='lightgrey'),
    yaxis=dict(gridcolor='lightgrey', zeroline=True, zerolinewidth=2, zerolinecolor='lightgrey')
)

# Save plot
try:
    fig.write_html(output_html, auto_open=False)
    print(f"Interactive plot saved to: '{output_html}'")
    fig.show()
except Exception as e:
    print(f"Error saving or displaying plot: {e}")

print("--- Visualization Complete ---")
