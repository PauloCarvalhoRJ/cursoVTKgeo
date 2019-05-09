#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <QString>
#include <QColor>
#include <vtkSmartPointer.h>

/*! The available color tables. */
enum class ColorTable : uint {
    RAINBOW = 0, /*!< The classic color table mapping low values to bluer hues and high values to redder hues. */
    SEISMIC = 1  /*!< Color table mapping low values to bluer hues and high values to redder hues with white in the middle. */
};

class vtkLookupTable;

/**
 * @brief The Util class organizes system-wide utilitary functions.
 */
class Util
{
public:

    /**
     * @brief Loads data from a GEO-EAS file.
     */
    static std::vector< std::vector< double > > loadGEOEAS( const QString path );

    /**
     * @brief Returns the first number in the given text file line.
     */
    static uint getFirstNumber(const QString line);

    /** Split function specialized to tokenize data lines of GEO-EAS files.
     *  @note This is not a generic tokenizer, so do not use for other applications.
     *        Use tokenizeWithQuotes() for generic tokenization (slower).
     */
    static void fastSplit(const QString lineGEOEAS, QStringList& list);

    /** Returns a color table object given its code. */
    static vtkSmartPointer<vtkLookupTable> getColorTable( ColorTable ct, double min, double max);

    /** Makes a color table for categorical values.
     * @param table A vector of pairs of a category code (an integer) and colors that will be used to paint the codes with.
     */
    static vtkSmartPointer<vtkLookupTable> getCategoricalColorTable(const std::vector< std::pair< uint, QColor > > &table );

    /** Converts a sequential index into a grid address following the GEO-EAS convetion (first i, then j, then k). */
    static void indexToIJK(uint index, int nI, int nJ, uint &i, uint &j, uint &k);

    /** Converts a grid address following the GEO-EAS convetion (first i, then j, then k).
     * into world coordinates.
     */
    static void IJKtoXYZ( uint i, uint j, uint k,
                          double x0, double y0, double z0,
                          double dx, double dy, double dz,
                          double &x, double &y, double &z );

private:
    /** The classic color table mapping lower values to bluer hues and high values to redder hues. */
    static vtkSmartPointer<vtkLookupTable> getClassicRainbow( double min, double max );

    /** The color table mapping lower values to bluer hues and high values to redder hues with white in the middle. */
    static vtkSmartPointer<vtkLookupTable> getSeismic( double min, double max );
};

#endif // UTIL_H
