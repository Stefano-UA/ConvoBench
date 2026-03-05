# <===========================================================================================>

#    ██████╗ ██████╗ ███╗   ███╗██████╗  █████╗ ██████╗  █████╗ ████████╗██╗██╗   ██╗███████╗
#   ██╔════╝██╔═══██╗████╗ ████║██╔══██╗██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝██║██║   ██║██╔════╝
#   ██║     ██║   ██║██╔████╔██║██████╔╝███████║██████╔╝███████║   ██║   ██║██║   ██║█████╗
#   ██║     ██║   ██║██║╚██╔╝██║██╔═══╝ ██╔══██║██╔══██╗██╔══██║   ██║   ██║╚██╗ ██╔╝██╔══╝
#   ╚██████╗╚██████╔╝██║ ╚═╝ ██║██║     ██║  ██║██║  ██║██║  ██║   ██║   ██║ ╚████╔╝ ███████╗
#    ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═══╝  ╚══════╝

# <===========================================================================================>
#                     Dual Benchmark Comparison & Architecture Parity
# <===========================================================================================>
#  Imports:
# <===========================================================================================>
import os
import sys
import numpy as np
import pandas as pd
import seaborn as sns
import plotly.express as px
import matplotlib.pyplot as plt
# <===========================================================================================>
#  Configuration & Constants:
# <===========================================================================================>
#  Script working directory
# <===========================================================================================>
WKDIR = os.path.dirname(os.path.abspath(sys.argv[0] if __name__ == '__main__' else __file__))
# <===========================================================================================>
#  Input data CSV paths
# <===========================================================================================>
CSV_A = os.path.join(WKDIR, '../benchmarks_A.csv')
CSV_B = os.path.join(WKDIR, '../benchmarks_B.csv')
# <===========================================================================================>
#  Output directory for visualizations
# <===========================================================================================>
OUTDIR = os.path.join(WKDIR, 'comparative/')
# <===========================================================================================>
#  Names for the machines (Labels)
# <===========================================================================================>
NAME_A = "Machine A"
NAME_B = "Machine B"
# <===========================================================================================>
#  Target Metric for analysis
# <===========================================================================================>
TARGET_CONV = 'Conv_s'
# <===========================================================================================>
#  Figure sizes
# <===========================================================================================>
FIGSIZE = (16, 8)
# <===========================================================================================>
#  Functions:
# <===========================================================================================>
def ensure_dir(d):
    if not os.path.exists(d):
        os.makedirs(d)
# <===========================================================================================>
#  Code:
# <===========================================================================================>
#  Initialize output directory
# <===========================================================================================>
ensure_dir(OUTDIR)
# <===========================================================================================>
#  Read input CSVs
# <===========================================================================================>
try:
    df_a = pd.read_csv(CSV_A, sep=';')
    df_b = pd.read_csv(CSV_B, sep=';')
except FileNotFoundError as e:
    print(f"[ERROR] Required CSV not found: {e}")
    sys.exit(1)
# <===========================================================================================>
#  Merge Datasets (Strict inner join on compilation axes)
# <===========================================================================================>
CATEGORICAL = ['O', 'ARCH', 'VEC', 'ALIG', 'IMG', 'KER', 'MINC', 'E', 'FM', 'S']
CATEGORICAL = [c for c in CATEGORICAL if c in df_a.columns and c in df_b.columns]
df_merged = pd.merge(df_a, df_b, on=CATEGORICAL, suffixes=('_A', '_B'))
if df_merged.empty:
    print("[ERROR] Merged dataset is empty. Check if categorical columns match.")
    sys.exit(1)
# <===========================================================================================>
#  Compute relative metrics
# <===========================================================================================>
#  Speedup Ratio: > 1 means Machine A is faster (took less time than B)
df_merged['Speedup_A_over_B'] = df_merged[f'{TARGET_CONV}_B'] / df_merged[f'{TARGET_CONV}_A']
# <===========================================================================================>
#  Set Matplotlib/Seaborn style
# <===========================================================================================>
plt.style.use('seaborn-v0_8-whitegrid')
sns.set_palette("husl")
# <===========================================================================================>
#  PLOT 1: Speedup Parallel Categories (Plotly)
# <===========================================================================================>
print(" -> Generating Parallel Categories Map...")
# Sort the dataframe by Speedup to organize the ribbons inside the plot
df_sorted = df_merged.sort_values(by='Speedup_A_over_B', ascending=False)
fig_pc = px.parallel_categories(
    color='Speedup_A_over_B',
    data_frame=df_sorted,
    dimensions=CATEGORICAL,
    title=f"Compilation Flags vs Speedup ({NAME_A} over {NAME_B})",
    color_continuous_scale=px.colors.diverging.RdBu_r,
    color_continuous_midpoint=1.0 # Centered around tie
)
# Update layout to dark mode and set white text for legibility
fig_pc.update_layout(plot_bgcolor='#111111', paper_bgcolor='#111111', font_color='white')
fig_pc.write_html(os.path.join(OUTDIR, 'parallel_categories_speedup.html'))
fig_pc.write_image(os.path.join(OUTDIR, 'parallel_categories_speedup.svg'))
# <===========================================================================================>
#  PLOT 2: Microarchitecture Scaling (Grouped Boxplot)
# <===========================================================================================>
print(" -> Generating Microarchitecture Scaling Plot...")
plt.figure(figsize=FIGSIZE)
ax_scale = sns.boxplot(
    data=df_merged,
    x='O',
    y='Speedup_A_over_B',
    hue='VEC',
    palette='muted',
    showfliers=False,
    width=0.7
)
# Neutral axis (Speedup = 1.0)
plt.axhline(1.0, color='black', linestyle='--', linewidth=2, label='Hardware Parity (Tie)')
plt.title(f'Microarchitecture Scaling: Optimization vs Vectorization', fontsize=16, fontweight='bold', pad=20)
plt.xlabel('Optimization Level (O)', fontsize=14)
plt.ylabel(f'Speedup Ratio ({NAME_A} / {NAME_B})', fontsize=14)
plt.legend(title='Vectorization (VEC)', fontsize=12, title_fontsize=12, loc='upper left')
plt.tight_layout()
plt.savefig(os.path.join(OUTDIR, 'microarch_scaling.png'), dpi=300)
plt.close()
# <===========================================================================================>
#  PLOT 3: Global Distribution & Absolute Parity (Violin + Swarm)
# <===========================================================================================>
print(" -> Generating Global Distribution Plot...")
# Melt dataframe for seaborn categorical plotting
df_melted = pd.melt(
    df_merged,
    id_vars=CATEGORICAL,
    value_vars=[f'{TARGET_CONV}_A', f'{TARGET_CONV}_B'],
    var_name='Machine',
    value_name='Execution Time (s)'
)
# Clean machine names for legend
df_melted['Machine'] = df_melted['Machine'].map({f'{TARGET_CONV}_A': NAME_A, f'{TARGET_CONV}_B': NAME_B})
plt.figure(figsize=FIGSIZE)
# Split violin plot showing density
sns.violinplot(
    data=df_melted,
    x='VEC',
    y='Execution Time (s)',
    hue='Machine',
    split=True,
    inner=None,
    palette=['#3498db', '#e74c3c'],
    alpha=0.6
)
# Overlay absolute minima points
sns.swarmplot(
    data=df_melted,
    x='VEC',
    y='Execution Time (s)',
    hue='Machine',
    dodge=True,
    size=3,
    palette=['#2980b9', '#c0392b'],
    legend=False,
    alpha=0.7
)
# Apply log scale to compress massive differences between O0 and O3
plt.yscale('log')
plt.title('Global Performance Distribution & Vectorization Ceiling', fontsize=16, fontweight='bold', pad=20)
plt.xlabel('Vectorization Strategy (VEC)', fontsize=14)
plt.ylabel('Execution Time (s) [Log Scale]', fontsize=14)
plt.legend(title='Machine', fontsize=12)
plt.tight_layout()
plt.savefig(os.path.join(OUTDIR, 'global_distribution.png'), dpi=300)
plt.close()
# <===========================================================================================>
#  PLOT 4: Speedup S-Curve
# <===========================================================================================>
print(" -> Generating Speedup S-Curve...")
plt.figure(figsize=FIGSIZE)
# Sort the dataset by Speedup ratio to generate the monotonic S-Curve
df_sorted_s = df_merged.sort_values('Speedup_A_over_B').reset_index(drop=True)
sns.scatterplot(
    x=df_sorted_s.index,
    y=df_sorted_s['Speedup_A_over_B'],
    hue=df_sorted_s['VEC'],
    palette='Set1',
    s=80,
    edgecolor='black',
    alpha=0.8
)
# Neutral axis (Speedup = 1.0)
plt.axhline(1.0, color='black', linestyle='--', linewidth=3, label='Hardware Parity (Tie)')
plt.title(f'Relative Acceleration (S-Curve): {NAME_A} over {NAME_B}', fontsize=16, fontweight='bold', pad=20)
plt.xlabel('Compilation Permutations (Sorted by Speedup)', fontsize=14)
plt.ylabel(f'Speedup Factor', fontsize=14)
plt.legend(title='Vectorization (VEC)', fontsize=12)
plt.tight_layout()
plt.savefig(os.path.join(OUTDIR, 'speedup_scurve.png'), dpi=300)
plt.close()
# <===========================================================================================>
#  PLOT 5: Dominance Distribution (Pie Chart)
# <===========================================================================================>
print(" -> Generating Dominance Pie Chart...")
# Define threshold for a tie (e.g., within 1% difference)
TIE_THRESHOLD = 0.01
df_merged['Winner'] = 'Tie'
df_merged.loc[df_merged['Speedup_A_over_B'] > (1.0 + TIE_THRESHOLD), 'Winner'] = NAME_A
df_merged.loc[df_merged['Speedup_A_over_B'] < (1.0 - TIE_THRESHOLD), 'Winner'] = NAME_B
winner_counts = df_merged['Winner'].value_counts()
colors = {NAME_A: '#3498db', NAME_B: '#e74c3c', 'Tie': '#95a5a6'}
pie_colors = [colors.get(w, '#333333') for w in winner_counts.index]
plt.figure(figsize=(10, 8))
plt.pie(
    winner_counts,
    labels=winner_counts.index,
    autopct='%1.1f%%',
    startangle=140,
    colors=pie_colors,
    wedgeprops={'edgecolor': 'black', 'linewidth': 1.5},
    textprops={'fontsize': 14, 'fontweight': 'bold'}
)
plt.title('Global Dominance (Win/Loss Ratio)', fontsize=18, fontweight='bold', pad=20)
plt.tight_layout()
plt.savefig(os.path.join(OUTDIR, 'dominance_pie.png'), dpi=300)
plt.close()
# <===========================================================================================>
#  PLOT 6: Speedup Density Distribution
# <===========================================================================================>
print(" -> Generating Speedup Density Plot...")
plt.figure(figsize=FIGSIZE)
ax_density = sns.histplot(
    data=df_merged,
    x='Speedup_A_over_B',
    kde=True,
    bins=50,
    color='purple',
    alpha=0.4,
    line_kws={'linewidth': 3}
)
plt.axvline(1.0, color='black', linestyle='--', linewidth=3, label='Hardware Parity (Tie)')
# Shade regions if KDE is available
if len(ax_density.lines) > 0:
    kdeline = ax_density.lines[0]
    x_kde = kdeline.get_xdata()
    y_kde = kdeline.get_ydata()
    plt.fill_between(x_kde, 0, y_kde, where=(x_kde > 1.0), color='#3498db', alpha=0.3, label=f'{NAME_A} Faster')
    plt.fill_between(x_kde, 0, y_kde, where=(x_kde <= 1.0), color='#e74c3c', alpha=0.3, label=f'{NAME_B} Faster')
plt.title(f'Speedup Density Distribution ({NAME_A} / {NAME_B})', fontsize=16, fontweight='bold', pad=20)
plt.xlabel('Speedup Factor (> 1.0 favors Machine A)', fontsize=14)
plt.ylabel('Frequency (Number of Configurations)', fontsize=14)
plt.legend(fontsize=12)
plt.tight_layout()
plt.savefig(os.path.join(OUTDIR, 'speedup_density.png'), dpi=300)
plt.close()
# <===========================================================================================>
print(" [OK] Comparative analysis complete.")
# <===========================================================================================>