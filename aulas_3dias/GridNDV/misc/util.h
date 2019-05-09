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


    /** Performs a reliable comparison of floating-pointing numbers. */
    inline static bool almostEqual2sComplement(double A, double B, int maxUlps)
    {
        int64_t aLong = *reinterpret_cast<int64_t *>(&A); // use the raw bytes from the
                                                          // double to make a long int
                                                          // value (type punning)
        // Make aLong lexicographically ordered as a twos-complement long
        if (aLong < 0)
            aLong = 0x8000000000000000 - aLong;
        // Make bLong lexicographically ordered as a twos-complement long
        int64_t bLong = *reinterpret_cast<int64_t *>(&B); // use the raw bytes from the
                                                          // double to make a long int
                                                          // value (type punning)
        if (bLong < 0)
            bLong = 0x8000000000000000 - bLong;
        int64_t longDiff = (aLong - bLong) & 0x7FFFFFFFFFFFFFFF;
        if (longDiff <= maxUlps)
            return true;
        return false;
    }

private:
    /** The classic color table mapping lower values to bluer hues and high values to redder hues. */
    static vtkSmartPointer<vtkLookupTable> getClassicRainbow( double min, double max );

    /** The color table mapping lower values to bluer hues and high values to redder hues with white in the middle. */
    static vtkSmartPointer<vtkLookupTable> getSeismic( double min, double max );
};

#endif // UTIL_H
