#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <QString>

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
};

#endif // UTIL_H
