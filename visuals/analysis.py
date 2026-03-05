# <=============================================================>

#    █████╗ ███╗   ██╗ █████╗ ██╗  ██╗   ██╗███████╗██╗███████╗
#   ██╔══██╗████╗  ██║██╔══██╗██║  ╚██╗ ██╔╝██╔════╝██║██╔════╝
#   ███████║██╔██╗ ██║███████║██║   ╚████╔╝ ███████╗██║███████╗
#   ██╔══██║██║╚██╗██║██╔══██║██║    ╚██╔╝  ╚════██║██║╚════██║
#   ██║  ██║██║ ╚████║██║  ██║███████╗██║   ███████║██║███████║
#   ╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝╚══════╝╚═╝   ╚══════╝╚═╝╚══════╝

# <=============================================================>
#         Single Benchmark Analysis & Feature Importance
# <=============================================================>
#  Imports:
# <=============================================================>
import os
import sys
import numpy as np
import pandas as pd
import seaborn as sns
import plotly.express as px
import matplotlib.pyplot as plt
# <=============================================================>
#  Configuration & Constants:
# <=============================================================>
#  Script working directory
# <=============================================================>
WKDIR = os.path.dirname(os.path.abspath(sys.argv[0] if __name__ == '__main__' else __file__))
# <=============================================================>
#  Input data CSV path
# <=============================================================>
CSV_IN = os.path.join(WKDIR, '../benchmarks.csv')
# <=============================================================>
#  Output directory for visualizations
# <=============================================================>
OUTDIR = os.path.join(WKDIR, 'analysis/')
# <=============================================================>
#  Target Metric for analysis
# <=============================================================>
TARGET = 'Total_s'
TARGET_CONV = 'Conv_s'
# <=============================================================>
#  Figure sizes
# <=============================================================>
FIGSIZE = (16, 8)
# <=============================================================>
#  Functions:
# <=============================================================>
def ensure_dir(d):
    if not os.path.exists(d):
        os.makedirs(d)
# <=============================================================>
#  Code:
# <=============================================================>
#  Initialize output directory
# <=============================================================>
ensure_dir(OUTDIR)
# <=============================================================>
#  Read input CSV
# <=============================================================>
try:
    df = pd.read_csv(CSV_IN, sep=';')
except FileNotFoundError:
    print(f"[ERROR] Could not find {CSV_IN}")
    sys.exit(1)
# <=============================================================>
#  Define categorical space
# <=============================================================>
CATEGORICAL = ['O', 'ARCH', 'VEC', 'ALIG', 'IMG', 'KER', 'MINC', 'E', 'FM', 'S']
# Drop missing columns gracefully if running against a subset of columns
CATEGORICAL = [c for c in CATEGORICAL if c in df.columns]
# <=============================================================>
#  Set Matplotlib/Seaborn style
# <=============================================================>
plt.style.use('seaborn-v0_8-whitegrid')
sns.set_palette("husl")
# <=============================================================>
#  PLOT 1: Parallel Categories (Plotly)
# <=============================================================>
print(" -> Generating Parallel Categories Map...")
# Sort the dataframe by Target to organize the ribbons inside the plot
df_sorted = df.sort_values(by=TARGET, ascending=False)
fig_pc = px.parallel_categories(
    color=TARGET,
    data_frame=df_sorted,
    dimensions=CATEGORICAL,
    title="Compilation Flags vs Execution Time",
    color_continuous_scale=px.colors.sequential.Inferno_r
)
# Update layout to dark mode and set white text for legibility
fig_pc.update_layout(plot_bgcolor='#111111', paper_bgcolor='#111111', font_color='white')
fig_pc.write_html(os.path.join(OUTDIR, 'paralell_categories.html'))
fig_pc.write_image(os.path.join(OUTDIR, 'paralell_categories.svg'))
# <=============================================================>
#  PLOT 2: Top 10 Fastest Configurations
# <=============================================================>
print(" -> Generating Top 10 Fastest Configurations Plot...")
# Create a unified string representation of each configuration
df['Config_String'] = df[CATEGORICAL].apply(
    lambda row: ' | '.join([f"{col}{val}" for col, val in row.items()]), axis=1
)
# Extract the 10 rows with the lowest total execution time
df_top10 = df.nsmallest(10, TARGET_CONV)
plt.figure(figsize=FIGSIZE)
# Generate a horizontal bar plot for readability of long labels
ax_top = sns.barplot(data=df_top10, y='Config_String', x=TARGET_CONV, hue='Config_String', palette='rocket', legend=False)
plt.title(f'Top Fastest Configurations by {TARGET_CONV}')
plt.xlabel('Execution Time (Seconds)')
plt.ylabel('Configuration')
# Annotate each bar with its exact execution time
for p in ax_top.patches:
    ax_top.annotate(
        format(p.get_width(), '.4f'),
        (p.get_width(), p.get_y() + p.get_height() / 2.),
        ha='left', va='center', xytext=(5, 0), textcoords='offset points', fontsize=10
    )
plt.tight_layout()
plt.savefig(os.path.join(OUTDIR, 'top_10_fastest.png'), dpi=600)
plt.close()
# <=============================================================>
#  PLOT 3: Amdahl's Law - Isolated Strong Scaling (VEC Ablation)
# <=============================================================>
print(" -> Generating Amdahl's Law Visualization...")
# Identify the absolute fastest configuration to extract the optimal fixed parameters
optimal_row = df.loc[df[TARGET].idxmin()]
# Freeze all hyperparameters except VEC to maintain a strictly constant T_seq
fixed_params = {col: optimal_row[col] for col in CATEGORICAL if col != 'VEC'}
# Create a boolean mask starting with all True
mask = pd.Series(True, index=df.index)
# Iteratively apply logical AND for exact matching, avoiding string parsing errors
for col, val in fixed_params.items():
    if pd.isna(val): mask &= df[col].isna()
    else: mask &= (df[col] == val)
df_amdahl = df[mask].copy()
if not df_amdahl.empty:
    # Baseline for this subset is the slowest VEC configuration
    baseline_amdahl = df_amdahl.loc[df_amdahl[TARGET].idxmax()]
    t_tot_base = baseline_amdahl[TARGET]
    t_conv_base = baseline_amdahl[TARGET_CONV]
    # Calculate parallelizable fraction strictly within this controlled hyperplane
    p = t_conv_base / t_tot_base
    # Compute empirical speedups
    s_local = t_conv_base / df_amdahl[TARGET_CONV]
    s_global = t_tot_base / df_amdahl[TARGET]
    # Generate theoretical projection
    s_theory = np.linspace(1, max(s_local.max() * 1.1, 5.0), 200)
    S_theory = 1 / ((1 - p) + (p / s_theory))
    max_S = 1 / (1 - p)
    plt.figure(figsize=FIGSIZE)
    sns.scatterplot(
        x=s_local,
        y=s_global,
        s=150,
        color='#e74c3c',
        edgecolor='black',
        label='Empirical Results',
        zorder=3
    )
    plt.plot(
        s_theory,
        S_theory,
        color='#2980b9',
        linewidth=3,
        linestyle='-',
        label=f'Theoretical Curve (p = {p:.4f})',
        zorder=2
    )
    plt.axhline(
        max_S,
        color='#2c3e50',
        linestyle='--',
        linewidth=2.5,
        label=f'Asymptotic Limit ({max_S:.2f}x)',
        zorder=1
    )
    # Text box indicating the frozen parameters to provide context to the reader
    fixed_str = '\n'.join([f"{k}: {v}" for k, v in fixed_params.items()])
    plt.text(0.02, 0.70, f"Fixed Parameters:\n{fixed_str}",
             transform=plt.gca().transAxes, fontsize=10, bbox=dict(facecolor='white', alpha=0.8), zorder=4)
    plt.title("Amdahl's Law: Vectorization Scaling", fontsize=16, fontweight='bold', pad=20)
    plt.xlabel(f"Local Execution Speedup (Parallelizable Time)", fontsize=14)
    plt.ylabel(f"Global Execution Speedup (Total Time)", fontsize=14)
    plt.legend(fontsize=12, loc='lower right', frameon=True, shadow=True)
    plt.tight_layout()
    plt.savefig(os.path.join(OUTDIR, 'amdahls_law.png'), dpi=300)
    plt.close()
else:
    print(" [WARNING] Not enough data variance to generate Amdahl's plot.")
# <=============================================================>
#  PLOT 4: Datatype Casting Impact (IMG vs KER)
# <=============================================================>
print(" -> Generating Datatype Interaction Plot...")
# Exclude MINC if it exists as it interferes with pure numerical casting analysis
df_cast = df.drop(columns=['MINC']) if 'MINC' in df.columns else df.copy()
plt.figure(figsize=FIGSIZE)
# Use a grouped boxplot to expose variance and extreme outliers in memory packing
ax_cast = sns.boxplot(
    data=df_cast,
    x='IMG',
    y=TARGET_CONV,
    hue='KER',
    palette='muted',
    showfliers=True, # Show outliers to detect catastrophic casting combinations
    width=0.6
)
# Apply log scale since bad datatype combinations might skew the visual linearly
plt.yscale('log')
plt.title(f'Datatype Interaction: Image vs Kernel', fontsize=16, fontweight='bold', pad=20)
plt.xlabel('Image Datatype (IMG)', fontsize=14)
plt.ylabel(f'{TARGET_CONV} (Seconds) [Log Scale]', fontsize=14)
plt.legend(title='Kernel Datatype (KER)', fontsize=12, title_fontsize=12)
plt.tight_layout()
plt.savefig(os.path.join(OUTDIR, 'datatype_interaction.png'), dpi=300)
plt.close()
# <=============================================================>
#  PLOT 5: Aggregation Bias Exposure (VEC Violin + Swarm)
# <=============================================================>
print(" -> Generating VEC Aggregation Bias Plot...")
plt.figure(figsize=FIGSIZE)
# Use violin plot to show probability density (bimodal detection)
sns.violinplot(
    data=df,
    x='VEC',
    y=TARGET_CONV,
    inner=None, # Remove internal boxplot to clear space for swarm
    color='lightgray',
    alpha=0.5
)
# Overlay swarm plot to show actual data points distribution
sns.swarmplot(
    data=df,
    x='VEC',
    y=TARGET_CONV,
    size=3,
    palette='dark:red',
    hue='VEC',
    legend=False,
    alpha=0.7
)
plt.title(f'Vectorization Aggregation Bias: Density and Absolute Minima', fontsize=16, fontweight='bold', pad=20)
plt.xlabel('Vectorization Strategy (VEC)', fontsize=14)
plt.ylabel(f'{TARGET_CONV} (Seconds)', fontsize=14)
plt.tight_layout()
plt.savefig(os.path.join(OUTDIR, 'vectorization_violin.png'), dpi=300)
plt.close()
# <=============================================================>
#  PLOT 6: Global Parameter Spectrum (Min/Median/Max Dumbbell)
# <=============================================================>
print(" -> Generating Global Parameter Spectrum...")
ranged_data = []
# Calculate absolute Min, Median, and Max for every option of every flag
for flag in CATEGORICAL:
    grouped = df.groupby(flag)[TARGET_CONV]
    for option, group in grouped:
        ranged_data.append({
            'Flag': flag,
            'Option': f"{flag}: {option}",
            'Min': group.min(),
            'Median': group.median(),
            'Max': group.max()
        })
df_ranged = pd.DataFrame(ranged_data)
# Sort strictly by the Absolute Minimum to expose the true hardware ceiling enablers
df_ranged = df_ranged.sort_values(by='Min', ascending=False)
plt.figure(figsize=(15, len(df_ranged) * 0.4 + 2)) # Dynamic height based on parameter count
# Draw the range line connecting Min and Max
plt.hlines(y=df_ranged['Option'], xmin=df_ranged['Min'], xmax=df_ranged['Max'], color='gray', alpha=0.4, linewidth=3)
# Plot Max (Red), Median (Yellow), and Min (Green)
plt.scatter(df_ranged['Max'], df_ranged['Option'], color='#e74c3c', s=100, label='Maximum Time (Worst Case)', zorder=3)
plt.scatter(df_ranged['Median'], df_ranged['Option'], color='#f1c40f', s=100, label='Median Time', zorder=3)
plt.scatter(df_ranged['Min'], df_ranged['Option'], color='#2ecc71', s=150, edgecolor='black', label='Absolute Minimum Time (Best Case)', zorder=4)
# Apply log scale for better readability of massive variance
plt.xscale('log')
plt.title(f'Parameter Execution Spectrum', fontsize=16, fontweight='bold', pad=20)
plt.xlabel(f'{TARGET_CONV} (Seconds) [Log Scale]', fontsize=14)
plt.ylabel('Flag Options', fontsize=14)
plt.legend(loc='lower left', frameon=True, shadow=True, fontsize=11)
plt.grid(axis='y', linestyle='--', alpha=0.3)
plt.tight_layout()
plt.savefig(os.path.join(OUTDIR, 'global_spectrum.png'), dpi=300)
plt.close()
# <=============================================================>
print(" [OK] Analysis complete.")
# <=============================================================>