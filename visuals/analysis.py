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
FIGSIZE = (20, 10)
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
    df = pd.read_csv(CSV_IN)
except FileNotFoundError:
    print(f"[ERROR] Could not find {CSV_IN}")
    sys.exit(1)
# <=============================================================>
#  Define categorical space
# <=============================================================>
CATEGORICAL = ['O', 'ARCH', 'VEC', 'ALIG', 'IMG', 'KER', 'E', 'FM', 'S']
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
fig_pc.write_image(os.path.join(OUTDIR, 'paralell_categories.png'))
# <=============================================================>
#  PLOT 2: Relative Improvement per Flag (Convolution Time)
# <=============================================================>
print(" -> Generating Relative Improvement Plot (Convolution Time)...")
improvement_data = []
# Calculate the mathematical improvement for each flag independently
for flag in CATEGORICAL:
    # Group by flag options and get the mean convolution time
    means = df.groupby(flag)[TARGET_CONV].mean()
    # Establish the baseline as the arithmetic mean of all options within this flag
    baseline_time = means.mean()
    for option, val in means.items():
        # Calculate relative performance delta percentage: ((Baseline - Value) / Baseline) * 100
        # Positive % means faster than average, negative % means slower than average
        imp_percent = ((baseline_time - val) / baseline_time) * 100
        improvement_data.append({
            'Flag': flag,
            'Option': f"{flag}: {option}",
            'Improvement (%)': imp_percent
        })
# Convert to DataFrame and sort for structured visualization
df_imp = pd.DataFrame(improvement_data)
df_imp = df_imp.sort_values(by=['Flag', 'Improvement (%)'], ascending=[True, True])
plt.figure(figsize=FIGSIZE)
# Generate a horizontal bar plot grouped by flags
ax = sns.barplot(
    data=df_imp,
    y='Option',
    hue='Flag',
    dodge=False,
    palette='viridis',
    x='Improvement (%)'
)
# Apply symmetric logarithmic scale to handle both negative and positive small deltas
plt.xscale('symlog', linthresh=0.5)
plt.title('Convolution Performance Relative Delta vs Flag Mean')
plt.xlabel('Performance Delta (%) [>0 = Faster, <0 = Slower]')
plt.ylabel('Flag Options')
# Add a vertical line at 0% to clearly show the mean baseline anchor
plt.axvline(0, color='black', linewidth=1.5, linestyle='--')
# Remove the legend as it's redundant
plt.legend([],[], frameon=False)
plt.tight_layout()
plt.savefig(os.path.join(OUTDIR, 'relative_improvement.png'), dpi=300)
plt.close()
# <=============================================================>
#  PLOT 3: Top 10 Fastest Configurations
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
ax_top = sns.barplot(data=df_top10, y='Config_String', x=TARGET_CONV, palette='rocket')
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
plt.savefig(os.path.join(OUTDIR, 'top_fastest.png'), dpi=600)
plt.close()
# <=============================================================>
print(" [OK] Analysis complete.")
# <=============================================================>