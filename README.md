# GLoSS-RT

A geospatial line of sight evaluation system for antenna LoS evaluations in small to large scale network simulations.

## Installation

### Step 1: Install System Dependencies

**IMPORTANT:** You must install GDAL system libraries **before** installing glossrt.

#### Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install libgdal-dev gdal-bin
```

#### macOS:
```bash
brew install gdal
```

#### Conda (Recommended):
```bash
conda create -n gloss-env python=3.11
conda activate gloss-env
conda install -c conda-forge gdal
```

### Step 2: Install glossrt

```bash
pip install glossrt
```

Or from TestPyPI:
```bash
pip install --index-url https://test.pypi.org/simple/ --extra-index-url https://pypi.org/simple/ glossrt
```

## Quick Start

```python
import gloss

# Initialize
gloss.initialize("antennas.csv", "elevation.tif", "ground_elevation.tif")

# Compute
results = gloss.compute()

# Save results
gloss.saveResults(results)
```

## Development

### Build from source

```bash
git clone https://github.com/yourusername/GLoSS.git
cd GLoSS
pip install -e .
```

## Requirements

- Python >= 3.7
- GDAL >= 3.0.0 (system library)
- fmt library
- CMake >= 3.4
- C++17 compatible compiler

## License

MIT License