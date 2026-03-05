# <===========================================================================================>

#   ████████╗██████╗ ██╗███╗   ██╗██╗████████╗██╗   ██╗
#   ╚══██╔══╝██╔══██╗██║████╗  ██║██║╚══██╔══╝╚██╗ ██╔╝
#      ██║   ██████╔╝██║██╔██╗ ██║██║   ██║    ╚████╔╝
#      ██║   ██╔══██╗██║██║╚██╗██║██║   ██║     ╚██╔╝
#      ██║   ██║  ██║██║██║ ╚████║██║   ██║      ██║
#      ╚═╝   ╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝      ╚═╝

# <===========================================================================================>
#               Trinity Benchmark Comparison & Architecture Parity (3-Way)
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
CSV_C = os.path.join(WKDIR, '../benchmarks_C.csv')
# <===========================================================================================>
#  Output directory for visualizations
# <===========================================================================================>
OUTDIR = os.path.join(WKDIR, 'trinity/')
# <===========================================================================================>
#  Names for the machines (Labels)
# <===========================================================================================>
NAME_A = "Machine A"
NAME_B = "Machine B"
NAME_C = "Machine C"
# <===========================================================================================>
#  Colors
# <===========================================================================================>
COLOR_A = '#3498db' # Blue
COLOR_B = '#e74c3c' # Red
COLOR_C = '#2ecc71' # Green
COLOR_TIE = '#95a5a6' # Gray
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
    df_c = pd.read_csv(CSV_C, sep=';')
except FileNotFoundError as e:
    print(f"[ERROR] Required CSV not found: {e}")
    sys.exit(1)
# <===========================================================================================>
#  Merge Datasets (Strict inner join on compilation axes)
# <===========================================================================================>
CATEGORICAL = ['O', 'ARCH', 'VEC', 'ALIG', 'IMG', 'KER', 'MINC', 'E', 'FM', 'S']
CATEGORICAL = [c for c in CATEGORICAL if c in df_a.columns and c in df_b.columns and c in df_c.columns]
# Iterative merge
df_merged = pd.merge(df_a, df_b, on=CATEGORICAL, suffixes=('_A', '_B'))
df_merged = pd.merge(df_merged, df_c, on=CATEGORICAL)
df_merged.rename(columns={TARGET_CONV: f'{TARGET_CONV}_C'}, inplace=True) # Fix suffix for C
if df_merged.empty:
    print("[ERROR] Merged dataset is empty. Check if categorical columns match.")
    sys.exit(1)
# <===========================================================================================>
#  Compute relative metrics & Winner
# <===========================================================================================>
TIE_THRESHOLD = 0.01 # 1% difference is considered a tie
def determine_winner(row):
    times = {
        NAME_A: row[f'{TARGET_CONV}_A'],
        NAME_B: row[f'{TARGET_CONV}_B'],
        NAME_C: row[f'{TARGET_CONV}_C']
    }
    min_time = min(times.values())
    # Check how many are within the tie threshold
    close_competitors = [name for name, time in times.items() if time <= min_time * (1.0 + TIE_THRESHOLD)]
    if len(close_competitors) > 1:
        return 'Tie'
    return close_competitors[0]
df_merged['Winner'] = df_merged.apply(determine_winner, axis=1)
# Melt dataframe for seaborn categorical plotting
df_melted = pd.melt(
    df_merged,
    id_vars=CATEGORICAL + ['Winner'],
    value_vars=[f'{TARGET_CONV}_A', f'{TARGET_CONV}_B', f'{TARGET_CONV}_C'],
    var_name='Machine',
    value_name='Execution Time (s)'
)
# Clean machine names for legend
df_melted['Machine'] = df_melted['Machine'].map({
    f'{TARGET_CONV}_A': NAME_A,
    f'{TARGET_CONV}_B': NAME_B,
    f'{TARGET_CONV}_C': NAME_C
})
# <===========================================================================================>
#  Set Matplotlib/Seaborn style
# <===========================================================================================>
plt.style.use('seaborn-v0_8-whitegrid')
palette_dict = {NAME_A: COLOR_A, NAME_B: COLOR_B, NAME_C: COLOR_C, 'Tie': COLOR_TIE}
# <===========================================================================================>
#  PLOT 1: Winning Machine Parallel Categories (Plotly)
# <===========================================================================================>
print(" -> Generating Parallel Categories Map...")
# Map Winner to numerical values for discrete color mapping in Plotly
winner_color_map = {NAME_A: 0, NAME_B: 1, NAME_C: 2, 'Tie': 3}
df_merged['Winner_Code'] = df_merged['Winner'].map(winner_color_map)
# Custom discrete color scale
color_scale = [
    [0.00, COLOR_A], [0.25, COLOR_A],
    [0.25, COLOR_B], [0.50, COLOR_B],
    [0.50, COLOR_C], [0.75, COLOR_C],
    [0.75, COLOR_TIE], [1.00, COLOR_TIE]
]
fig_pc = px.parallel_categories(
    color='Winner_Code',
    data_frame=df_merged,
    dimensions=CATEGORICAL,
    title=f"Compilation Flags vs Absolute Winner",
    color_continuous_scale=color_scale
)
# Hide the continuous color bar as it's discrete
fig_pc.update_layout(
    plot_bgcolor='#111111',
    paper_bgcolor='#111111',
    font_color='white',
    coloraxis_showscale=False,
    annotations=[
        dict(
            x=1.1, y=1.05, xref='paper', yref='paper', showarrow=False,
            text=f"<span style='color:{COLOR_A}'>█</span> {NAME_A}<br><span style='color:{COLOR_B}'>█</span> {NAME_B}<br><span style='color:{COLOR_C}'>█</span> {NAME_C}<br><span style='color:{COLOR_TIE}'>█</span> Tie",
            align="left", font=dict(size=14, color="white")
        )
    ]
)
fig_pc.write_html(os.path.join(OUTDIR, 'parallel_categories_winner.html'))
fig_pc.write_image(os.path.join(OUTDIR, 'parallel_categories_winner.svg'))
# <===========================================================================================>
#  PLOT 2: Microarchitecture Scaling (Grouped Boxplot)
# <===========================================================================================>
print(" -> Generating Microarchitecture Scaling Plot...")
plt.figure(figsize=FIGSIZE)
ax_scale = sns.boxplot(
    data=df_melted,
    x='O',
    y='Execution Time (s)',
    hue='Machine',
    palette=palette_dict,
    showfliers=False,
    width=0.7
)
plt.yscale('log')
plt.title(f'Microarchitecture Scaling: Optimization Levels', fontsize=16, fontweight='bold', pad=20)
plt.xlabel('Optimization Level (O)', fontsize=14)
plt.ylabel('Execution Time (s) [Log Scale]', fontsize=14)
plt.legend(title='Machine', fontsize=12, title_fontsize=12, loc='upper right')
plt.tight_layout()
plt.savefig(os.path.join(OUTDIR, 'microarch_scaling.png'), dpi=300)
plt.close()
# <===========================================================================================>
#  PLOT 3: Global Distribution & Absolute Parity (Violin + Swarm)
# <===========================================================================================>
print(" -> Generating Global Distribution Plot...")
plt.figure(figsize=FIGSIZE)
# Side-by-side violin plot showing density
sns.violinplot(
    data=df_melted,
    x='VEC',
    y='Execution Time (s)',
    hue='Machine',
    inner=None,
    palette=palette_dict,
    alpha=0.5,
    linewidth=1
)
# Overlay absolute minima points
sns.swarmplot(
    data=df_melted,
    x='VEC',
    y='Execution Time (s)',
    hue='Machine',
    dodge=True,
    size=3,
    palette=palette_dict,
    legend=False,
    alpha=0.8
)
plt.yscale('log')
plt.title('Global Performance Distribution & Vectorization Ceiling', fontsize=16, fontweight='bold', pad=20)
plt.xlabel('Vectorization Strategy (VEC)', fontsize=14)
plt.ylabel('Execution Time (s) [Log Scale]', fontsize=14)
plt.legend(title='Machine', fontsize=12, loc='upper right')
plt.tight_layout()
plt.savefig(os.path.join(OUTDIR, 'global_distribution.png'), dpi=300)
plt.close()
# <===========================================================================================>
#  PLOT 4: Execution Time S-Curve
# <===========================================================================================>
print(" -> Generating Execution Time S-Curve...")
plt.figure(figsize=FIGSIZE)
# Calculate mean execution time across all 3 to sort the X-axis monotonically
df_merged['Mean_Time'] = df_merged[[f'{TARGET_CONV}_A', f'{TARGET_CONV}_B', f'{TARGET_CONV}_C']].mean(axis=1)
df_sorted_s = df_merged.sort_values('Mean_Time').reset_index(drop=True)
# Plot A
plt.scatter(df_sorted_s.index, df_sorted_s[f'{TARGET_CONV}_A'], color=COLOR_A, s=40, label=NAME_A, alpha=0.7, edgecolor='none')
# Plot B
plt.scatter(df_sorted_s.index, df_sorted_s[f'{TARGET_CONV}_B'], color=COLOR_B, s=40, label=NAME_B, alpha=0.7, edgecolor='none')
# Plot C
plt.scatter(df_sorted_s.index, df_sorted_s[f'{TARGET_CONV}_C'], color=COLOR_C, s=40, label=NAME_C, alpha=0.7, edgecolor='none')
plt.yscale('log')
plt.title(f'Execution Time Spectrum (Sorted by Average Time)', fontsize=16, fontweight='bold', pad=20)
plt.xlabel('Compilation Permutations (Sorted by Difficulty/Time)', fontsize=14)
plt.ylabel(f'Execution Time (s) [Log Scale]', fontsize=14)
plt.legend(title='Machine', fontsize=12)
plt.tight_layout()
plt.savefig(os.path.join(OUTDIR, 'execution_scurve.png'), dpi=300)
plt.close()
# <===========================================================================================>
#  PLOT 5: Dominance Distribution (Pie Chart)
# <===========================================================================================>
print(" -> Generating Dominance Pie Chart...")
winner_counts = df_merged['Winner'].value_counts()
pie_colors = [palette_dict.get(w, '#333333') for w in winner_counts.index]
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
plt.title('Global Dominance (Win/Loss/Tie Ratio)', fontsize=18, fontweight='bold', pad=20)
plt.tight_layout()
plt.savefig(os.path.join(OUTDIR, 'dominance_pie.png'), dpi=300)
plt.close()
# <===========================================================================================>
#  PLOT 6: Performance Density Distribution (KDE)
# <===========================================================================================>
print(" -> Generating Execution Density Plot...")
plt.figure(figsize=FIGSIZE)
sns.kdeplot(data=df_merged, x=f'{TARGET_CONV}_A', color=COLOR_A, fill=True, alpha=0.3, linewidth=2, label=NAME_A)
sns.kdeplot(data=df_merged, x=f'{TARGET_CONV}_B', color=COLOR_B, fill=True, alpha=0.3, linewidth=2, label=NAME_B)
sns.kdeplot(data=df_merged, x=f'{TARGET_CONV}_C', color=COLOR_C, fill=True, alpha=0.3, linewidth=2, label=NAME_C)
plt.xscale('log') # Log scale because time distributions have a long tail
plt.title(f'Execution Time Density Distribution', fontsize=16, fontweight='bold', pad=20)
plt.xlabel('Execution Time (s) [Log Scale]', fontsize=14)
plt.ylabel('Density (Frequency of Configurations)', fontsize=14)
plt.legend(title='Machine', fontsize=12)
plt.tight_layout()
plt.savefig(os.path.join(OUTDIR, 'execution_density.png'), dpi=300)
plt.close()
# <===========================================================================================>
print(" [OK] Trinity comparative analysis complete.")
# <===========================================================================================>