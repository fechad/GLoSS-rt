from osgeo import gdal, osr
import sys

def view_map(map_path, resolution_m=None, max_memory_gb=8):
    import rasterio
    import rasterio.plot
    import matplotlib.pyplot as plt
    import numpy as np

    with rasterio.open(map_path) as tiff:
        # Get basic info without loading data
        print(f"Shape: {tiff.shape}")
        print(f"Data type: {tiff.dtypes[0]}")
        print(f"Bounds: {tiff.bounds}")
        print(f"Original resolution: ~1m per pixel")
        
        # Calculate memory requirements for full resolution
        bytes_per_pixel = np.dtype(tiff.dtypes[0]).itemsize
        full_size_gb = (tiff.width * tiff.height * bytes_per_pixel) / (1024**3)
        print(f"Full resolution would require ~{full_size_gb:.2f} GB of RAM")
        
        # Determine downsampling factor
        if resolution_m is None:
            # Auto-calculate based on memory limit (account for matplotlib overhead)
            effective_memory_gb = max_memory_gb * 0.3  # Use only 30% for raw data
            max_pixels_total = (effective_memory_gb * 1024**3) / bytes_per_pixel
            downsample_factor = max(int((tiff.width * tiff.height / max_pixels_total)**0.5), 1)
            actual_resolution = downsample_factor
            
            # Verify the actual memory usage
            actual_pixels = (tiff.width // downsample_factor) * (tiff.height // downsample_factor)
            actual_gb = (actual_pixels * bytes_per_pixel) / (1024**3)
            estimated_total_gb = actual_gb * 3.5  # Account for matplotlib overhead
            print(f"Auto-selected resolution: {actual_resolution}m per pixel (downsampled by {downsample_factor}x)")
            print(f"Raw data: {actual_gb:.2f} GB, estimated total usage: {estimated_total_gb:.2f} GB")
        else:
            downsample_factor = max(int(resolution_m), 1)
            actual_resolution = downsample_factor
            
            # Check if this will exceed memory limit
            raw_gb = (tiff.width * tiff.height * bytes_per_pixel) / (downsample_factor**2) / (1024**3)
            estimated_total_gb = raw_gb * 3.5  # Account for matplotlib overhead
            print(f"Requested resolution: {actual_resolution}m per pixel")
            print(f"Raw data: {raw_gb:.2f} GB, estimated total usage: {estimated_total_gb:.2f} GB")
            
            if estimated_total_gb > max_memory_gb:
                response = input(f"Warning: This may use {estimated_total_gb:.2f} GB total RAM. Continue? (y/N): ")
                if response.lower() != 'y':
                    print("Operation cancelled.")
                    return
        
        # Read downsampled data
        print(f"Loading data at {actual_resolution}m resolution...")
        data = tiff.read(
            1,  # Read first band
            out_shape=(
                tiff.height // downsample_factor,
                tiff.width // downsample_factor
            ),
            resampling=rasterio.enums.Resampling.average
        )
        
        # Handle no-data values and calculate robust statistics
        nodata_value = tiff.nodata
        if nodata_value is not None:
            print(f"No-data value: {nodata_value}")
            valid_mask = (data != nodata_value) & (~np.isnan(data))
        else:
            # Check for common no-data values in elevation data
            valid_mask = (data > -9999) & (~np.isnan(data))
        
        valid_data = data[valid_mask]
        
        if len(valid_data) > 0:
            vmin = np.percentile(valid_data, 2)   # 2nd percentile
            vmax = np.percentile(valid_data, 98)  # 98th percentile
            print(f"Valid data range: {valid_data.min():.1f} to {valid_data.max():.1f}")
            print(f"Display range (2%-98%): {vmin:.1f} to {vmax:.1f}")
            
            # Mask no-data values for display
            data_masked = np.where(valid_mask, data, np.nan)
        else:
            vmin, vmax = None, None
            data_masked = data
        
        # Plot the data with memory optimization
        plt.figure(figsize=(12, 8))
        im = plt.imshow(data_masked, cmap='terrain', interpolation='nearest', vmin=vmin, vmax=vmax)
        plt.title(f"Montreal carte d'élévation ({actual_resolution}m/pixel)")
        plt.colorbar(im, label='Elevation (m)')
        
        # Force garbage collection before showing
        import gc
        gc.collect()
        
        plt.show()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python view_tiff.py <tiff_file_path> [resolution_in_meters] [max_memory_gb]")
        print("Examples:")
        print("  python view_tiff.py file.tif                    # Auto resolution")
        print("  python view_tiff.py file.tif 1                  # Full resolution (1m/pixel)")
        print("  python view_tiff.py file.tif 10                 # 10m per pixel")
        print("  python view_tiff.py file.tif 5 16               # 5m/pixel, allow 16GB RAM")
        sys.exit(1)
    
    map_path = sys.argv[1]
    resolution_m = int(sys.argv[2]) if len(sys.argv) > 2 else None
    max_memory_gb = float(sys.argv[3]) if len(sys.argv) > 3 else 8
    
    view_map(map_path, resolution_m, max_memory_gb)