#!/bin/bash
# <============================================>

#   ███████╗████████╗ █████╗ ██████╗ ████████╗
#   ██╔════╝╚══██╔══╝██╔══██╗██╔══██╗╚══██╔══╝
#   ███████╗   ██║   ███████║██████╔╝   ██║
#   ╚════██║   ██║   ██╔══██║██╔══██╗   ██║
#   ███████║   ██║   ██║  ██║██║  ██║   ██║
#   ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝

# <============================================>
#  Sets up the environment and makes the plots
# <============================================>
#  Configuration:
# <============================================>
WKDIR="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"
VENVDIR="${WKDIR}/../.venv"
# <============================================>
#  Create the python venv:
# <============================================>
echo "Creating Python virtual environment..."
python3 -m venv "$VENVDIR"
# <============================================>
#  Install the required dependencies:
# <============================================>
echo "Installing required Python packages in virtual environment..."
"${VENVDIR}/bin/pip" install --quiet --upgrade pip
"${VENVDIR}/bin/pip" install --quiet pandas numpy matplotlib seaborn plotly scikit-learn kaleido
# <============================================>
#  Execute the python script to make the plots:
# <============================================>
echo "Executing Python script..."
pushd "${WKDIR}/../visuals/" &>/dev/null
"${VENVDIR}/bin/python" "${WKDIR}/../visuals/analysis.py"
"${VENVDIR}/bin/python" "${WKDIR}/../visuals/comparative.py"
popd &>/dev/null
echo "Finished executing script!"
# <============================================>