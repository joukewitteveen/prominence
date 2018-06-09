#include "greader.hpp"
#include <fstream>
#include <boost/graph/graphml.hpp>
#include <boost/graph/graphviz.hpp>
#ifndef NOGDAL
#include <gdal_priv.h>
#include <boost/format.hpp>
#endif //NOGDAL

using namespace std;
using namespace boost;


#ifndef NOGDAL
bool from_gdal(GDALDataset* dataset, HeightMap& map) {
    format id_format("(%g, %g)");
    CPLErr error = CE_None;
    GDALRasterBand* raster;
    double* scanline;
    double geo_transform[6];
    double no_data;
    int xsize, ysize;

    if (dataset->GetGeoTransform(geo_transform) != CE_None) return false;
    xsize = dataset->GetRasterXSize();
    ysize = dataset->GetRasterYSize();

    raster = dataset->GetRasterBand(1); // Use the first raster band for now
    scanline = static_cast<double*>(CPLMalloc(sizeof(double) * xsize));
    no_data = raster->GetNoDataValue();

    map = HeightMap(xsize * ysize);
    for (int ypixel = 0; ypixel < ysize && error == CE_None; ++ypixel) {
        error = raster->RasterIO(GF_Read, 0, ypixel, xsize, 1,
                                 scanline, xsize, 1, GDT_Float64,
                                 0, 0, NULL);
        for (int xpixel = 0; xpixel < xsize && error == CE_None; ++xpixel) {
            auto v = vertex(ypixel * xsize + xpixel, map);
            map[v].id = str(id_format % (
                geo_transform[0] +
                (0.5 + xpixel) * geo_transform[1] +
                (0.5 + ypixel) * geo_transform[2]
            ) % (
                geo_transform[3] +
                (0.5 + xpixel) * geo_transform[4] +
                (0.5 + ypixel) * geo_transform[5]
            ));
            if (scanline[xpixel] != no_data)
                map[v].height = scanline[xpixel];

            // Grid edges
            if (xpixel != 0)
                add_edge(vertex(ypixel * xsize + xpixel - 1, map), v, map);
            if (ypixel != 0)
                add_edge(vertex((ypixel - 1) * xsize + xpixel, map), v, map);
        }
    }

    CPLFree(scanline);

    return error == CE_None;
}
#endif //NOGDAL


bool read_map(const string uri, HeightMap& map) {
    //TODO: This is a hack assuming string::npos == -1
    string const ext = uri.substr(uri.find_last_of('.') + 1);
    ifstream in(uri);
    if (!in) return false;

    dynamic_properties dp(ignore_other_properties);
    dp.property("node_id", get(&Feature::id, map));
    dp.property("height", get(&Feature::height, map));

    if (ext == "graphml" || ext == "xml") {
        read_graphml(in, map, dp);
        return true;
    } else if (ext == "gv" || ext == "dot")
        return read_graphviz(in, map, dp);
#ifndef NOGDAL
    else {
        in.close();
        GDALDataset* dataset;
        GDALAllRegister();
        dataset = static_cast<GDALDataset*>(GDALOpen(uri.c_str(), GA_ReadOnly));
        if (!dataset) return false;

        bool r = from_gdal(dataset, map);
        GDALClose(dataset);
        return r;
    }
#endif //NOGDAL
    return false;
}
