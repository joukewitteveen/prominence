#include "../heightmap.hpp"
#include <sstream>
#include <gdal_priv.h>

using namespace std;


vector<Location> adjacent_locations(Location l, HeightMap const& m) {
    vector<Location> adjacent;
    adjacent.reserve(4);

    if (l >= m.xsize)
        adjacent.push_back(l - m.xsize); // North
    if (l % m.xsize != 0)
        adjacent.push_back(l - 1); // West
    if ((l + 1) % m.xsize != 0)
        adjacent.push_back(l + 1); // East
    if (l + m.xsize < num_locations(m))
        adjacent.push_back(l + m.xsize); // South

    return adjacent;
}


string get_name(Location l, HeightMap const& m) {
    ostringstream name_stream;
    double x = 0.5 + (l % m.xsize), y = 0.5 + (l / m.xsize);

    name_stream << "(" << (
        m.geo_transform[0] + x * m.geo_transform[1] + y * m.geo_transform[2]
    ) << ", " << (
        m.geo_transform[3] + x * m.geo_transform[4] + y * m.geo_transform[5]
    ) << ")";

    return name_stream.str();
}


bool read_map(char const* uri, HeightMap& map) {
    CPLErr error = CE_None;
    GDALDataset* dataset;
    GDALRasterBand* raster;
    Height* scanline;
    Height no_data;
    int xsize, ysize;

    GDALAllRegister();
    dataset = static_cast<GDALDataset*>(GDALOpen(uri, GA_ReadOnly));
    if (!dataset) return false;

    xsize = dataset->GetRasterXSize();
    ysize = dataset->GetRasterYSize();
    map = HeightMap(xsize, ysize);

    if (dataset->GetGeoTransform(map.geo_transform) != CE_None) {
        GDALClose(dataset);
        return false;
    }

    raster = dataset->GetRasterBand(1); // Use the first raster band for now
    scanline = static_cast<Height*>(CPLMalloc(sizeof(Height) * xsize));
    no_data = raster->GetNoDataValue();

    for (int ypixel = 0; ypixel < ysize && error == CE_None; ++ypixel) {
        error = raster->RasterIO(GF_Read, 0, ypixel, xsize, 1,
                                 scanline, xsize, 1,
                                 sizeof(Height) == 4 ? GDT_Float32
                                                     : GDT_Float64,
                                 0, 0, NULL);
        for (int xpixel = 0; xpixel < xsize && error == CE_None; ++xpixel)
            if (scanline[xpixel] != no_data)
                map[ypixel * xsize + xpixel].height = scanline[xpixel];
    }

    CPLFree(scanline);
    GDALClose(dataset);

    return error == CE_None;
}
