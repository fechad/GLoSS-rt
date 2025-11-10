
#include <iostream>
#include <stdexcept>
#include "gdal_priv.h"
#include "ogr_spatialref.h"

class ElevationReader {
public:
    ElevationReader() = default;

    ElevationReader(std::string tiffFile) {
        // Register all GDAL drivers
        GDALAllRegister();

        // Open the file
        poDataset = (GDALDataset*)GDALOpen(tiffFile.c_str(), GA_ReadOnly);
        if (poDataset == nullptr) {
            throw std::runtime_error("Failed to open file.");
        }

        // Initialize the raster band
        poBand = poDataset->GetRasterBand(1);
        if (poBand == nullptr) {
            GDALClose(poDataset);
            throw std::runtime_error("Failed to get the raster band.");
        }

        // Get the geotransform
        if (poDataset->GetGeoTransform(adfGeoTransform) != CE_None) {
            GDALClose(poDataset);
            throw std::runtime_error("Failed to get the geotransform.");
        }

        // Initialize the coordinate transformation
        const char* pszSrcWKT = poDataset->GetProjectionRef();
        srcSRS.importFromEPSG(4326);  // WGS84
        dstSRS.importFromWkt(&pszSrcWKT);
        poCT = OGRCreateCoordinateTransformation(&srcSRS, &dstSRS);
        if (poCT == nullptr) {
            GDALClose(poDataset);
            throw std::runtime_error("Failed to create coordinate transformation.");
        }
    }

    ElevationReader(const ElevationReader&) = delete;
    ElevationReader& operator=(const ElevationReader&) = delete;

    ElevationReader(ElevationReader&& other) noexcept
        : poDataset(other.poDataset)
        , poBand(other.poBand)
        , srcSRS(std::move(other.srcSRS))
        , dstSRS(std::move(other.dstSRS))
        , poCT(other.poCT) 
    {
        std::copy(std::begin(other.adfGeoTransform), std::end(other.adfGeoTransform), std::begin(adfGeoTransform));
        other.poDataset = nullptr;
        other.poBand = nullptr;
        other.poCT = nullptr;
    }

    ElevationReader& operator=(ElevationReader&& other) noexcept {
        if (this != &other) {
            // Clean up existing resources
            if (poCT) OCTDestroyCoordinateTransformation(poCT);
            if (poDataset) GDALClose(poDataset);

            // Transfer ownership
            poDataset = other.poDataset;
            poBand = other.poBand;
            srcSRS = std::move(other.srcSRS);
            dstSRS = std::move(other.dstSRS);
            poCT = other.poCT;
            std::copy(std::begin(other.adfGeoTransform), std::end(other.adfGeoTransform), adfGeoTransform);

            // Nullify source
            other.poDataset = nullptr;
            other.poBand = nullptr;
            other.poCT = nullptr;
        }
        return *this;
    }

    ~ElevationReader() {
        if (poCT) {
            OCTDestroyCoordinateTransformation(poCT);
        }
        if (poDataset) {
            GDALClose(poDataset);
        }
    }

    float getElevation(double lat, double lon) {
        double x = lat;
        double y = lon;

        if (!poCT->Transform(1, &x, &y)) {
            std::cout << "Failed to transform coordinates." << std::endl;
            return -100.0;
            //throw std::runtime_error("Failed to transform coordinates.");
        }

        int pixel = static_cast<int>((x - adfGeoTransform[0]) / adfGeoTransform[1]);
        int line = static_cast<int>((y - adfGeoTransform[3]) / adfGeoTransform[5]);

        if (pixel < 0 || pixel >= poDataset->GetRasterXSize() ||
            line < 0 || line >= poDataset->GetRasterYSize()) {
            std::cout << "Pixel/Line coordinates are out of bounds." << std::endl;
            return -100.0;
            //throw std::out_of_range("Pixel/Line coordinates are out of bounds.");
        }

        float elevation = 0.0;
        CPLErr err = poBand->RasterIO(GF_Read, pixel, line, 1, 1, &elevation, 1, 1, GDT_Float32, 0, 0);

        if (err != CE_None) {
            std::cout << "Failed to read elevation value." << std::endl;
            return -100.0;
            //throw std::runtime_error("Failed to read elevation value.");
        }

        return elevation;
    }

private:
    GDALDataset* poDataset = nullptr;
    GDALRasterBand* poBand = nullptr;
    OGRSpatialReference srcSRS, dstSRS;
    OGRCoordinateTransformation* poCT = nullptr;
    double adfGeoTransform[6];
};

// int main() {
//     try {
//         ElevationReader reader("data/montreal/montreal.tif");

//         // Example usage
//         double lat = 45.5191;
//         double lon = -73.609;
//         float elevation = reader.getElevation(lat, lon);

//         std::cout << "Elevation at (" << lat << ", " << lon << "): " << elevation << " meters" << std::endl;

//     } catch (const std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return 1;
//     }

//     return 0;
// }
