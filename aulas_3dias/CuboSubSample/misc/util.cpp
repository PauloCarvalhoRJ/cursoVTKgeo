#include "misc/util.h"

#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QTextStream>

#include <vtkColorTransferFunction.h>
#include <vtkLookupTable.h>

#include <iostream>

std::vector<std::vector<double> > Util::loadGEOEAS(const QString path)
{
    std::vector<std::vector<double> > data;

    //QStringList list;
    int n_vars = 0;
    int var_count = 0;
    QFile file( path );
    file.open( QFile::ReadOnly );
    QTextStream in(&file);
    long bytesReadSofar = 0;
    QStringList valuesAsString;

    //Get data file size in bytes.
    QFileInfo fileInfo( file );
    uint64_t fileSize = fileInfo.size();

    uint iDataLineParsed = 0;

    uint nPushesBack = 0;

    uint nPopsBack = 0;

    for (int i = 0; !in.atEnd(); ++i)
    {
       //read file line by line
       QString line = in.readLine();

       //Clear the list with the tokenized data values as string
       valuesAsString.clear();

       //updates the progress
       bytesReadSofar += line.size() + 1; //account for line break char (in Windows may have 2, though.)

       //TODO: second line may contain other information in grid files, so it will fail for such cases.
       if( i == 0 ){} //first line is ignored
       else if( i == 1 ){ //second line is the number of variables
           n_vars = Util::getFirstNumber( line );
       } else if ( i > 1 && var_count < n_vars ){ //the variables names
           //list << line;
           ++var_count;
       } else { //parse lines containing data (must be within the target interval)
           //read line from the GSLib data file
           Util::fastSplit( line, valuesAsString );
           //if we are at the first data line to read...
           if( iDataLineParsed == 0 ){
               //...estimate the number of lines in the file
               uint nLinesExpectation = fileSize / ( line.size() + 2 ) + 200; //+2 -> line breaks in Windows; +200 -> make room for possible line length variation
               //now we can reserve capacity in the _data array to avoid minimize push_back calls and vector re-allocations/copies
               data = std::vector<std::vector<double> >( 1.1 * nLinesExpectation, std::vector<double>( n_vars, -424242.0 ) );
           }
           if( valuesAsString.size() != n_vars ){
               std::cerr << "ERROR: wrong number of values in line " << (i+1) << std::endl;
               std::cerr << "       expected: " << n_vars << ", found:" << valuesAsString.size() << std::endl;
           } else {
               //read each value along the line
               QStringList::Iterator it = valuesAsString.begin();
               for( int j = 0; it != valuesAsString.end(); ++it, ++j ){
                   //parse the double value
                   bool ok = true;
                   double value = (*it).toDouble( &ok );
                   if( !ok ){
                       std::cerr << "Util::loadGEOEAS(): error in data file (line " << (i+1) << "): cannot convert " << (*it).toStdString() << " to double." << std::endl;
                   }
                   //making sure there is room for the new data.
                   if( iDataLineParsed == data.size() ){
                       ++nPushesBack;
                       data.push_back( std::vector<double>( n_vars, -424242.0 ) );
                   }
                   //store the value in the data array.
                   data[iDataLineParsed][j] = value;
               }
               ++iDataLineParsed;
           }
       }
    }

    file.close();

    //remove possibly excess of data in pre-allocation of _data
    while( iDataLineParsed < data.size() ){
        ++nPopsBack;
        data.pop_back();
    }

    if( nPushesBack )
        std::cout << "Util::loadGEOEAS(): data array adjustment resulted in " << nPushesBack << " push(es)-back." << std::endl;

    if( nPopsBack )
        std::cout << "Util::loadGEOEAS(): data array adjustment resulted in " << nPopsBack << " pop(s)-back." << std::endl;

    return data;
}

uint Util::getFirstNumber(const QString line)
{
    uint result = 0;
    QRegularExpression re("(\\d+).*");
    QRegularExpressionMatch match = re.match( line );
    if( match.hasMatch() ){
        result = match.captured(1).toInt( );
    }
    return result;
}

void Util::fastSplit(const QString lineGEOEAS, QStringList & list)
{
    char token[100]; //100 characters is more than enough for a double in a text file.
    int iTokenChar = 0;
    int nchar = lineGEOEAS.length();
    char currentChar;

    //for each char of the line
    for( int i = 0; i < nchar; ++i){
        currentChar = lineGEOEAS[i].toLatin1(); //assumes no fancy unicode chars are in GEO-EAS data sets
        switch(currentChar){
            case '-': case '.': case '0': case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': case 'E': case 'e': case '+':
                token[ iTokenChar++ ] = currentChar;
                break;
            default:  //found a separator char (could be anything other than valid number characters)
                token[ iTokenChar ] = 0; //append null char
                if( iTokenChar > 0 ) //if token is not empty
                    list.push_back( token ); //adds the token to the string list
                iTokenChar = 0; //resets the token char counter
        }
    }

    //it is possible that the last token finishes the line
    if( iTokenChar > 0 ){ //if token is not empty
        token[ iTokenChar ] = 0; //append null char
        list.push_back( token ); //adds the token to the string list
    }
}

vtkSmartPointer<vtkLookupTable> Util::getColorTable(ColorTable ct, double min, double max)
{
    switch( ct ){
        case ColorTable::RAINBOW: return getClassicRainbow( min, max );
        case ColorTable::SEISMIC: return getSeismic( min, max );
    }
}

vtkSmartPointer<vtkLookupTable> Util::getCategoricalColorTable(const std::vector< std::pair< uint, QColor > > &table)
{
    //lambda that searches whether the code exists in table parameter variable
    auto fnCodeExists = [ table ]( uint code ) {
        for( const std::pair<uint, QColor>& pair : table )
            if( pair.first == code )
                return true;
        return false;
    };

    //lambda that fetches a color given a code in table parameter variable
    auto fnFetchColor = [ table ]( uint code ) {
        for( const std::pair<uint, QColor>& pair : table )
            if( pair.first == code )
                return pair.second;
        return QColor( Qt::white );
    };

    //determine the greatest categorical code
    uint maxCatCode = std::numeric_limits<uint>::min();
    for( const std::pair<uint, QColor>& pair : table )
        maxCatCode = std::max<uint>( maxCatCode, pair.first );

    //table color indexes must go from 0 to greatest facies code, without skipping values
    size_t tableSize = maxCatCode + 1;
    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(tableSize);


    //but assign only the codes defined in the category definition
    //which may be less than the total number of entries in the color table
    //this is a requirement by the way VTK's LUT work for categorical color tables
    for(size_t i = 0; i < tableSize; ++i)
    {
        if( fnCodeExists( i ) ){
            double rgb[3];
            QColor color = fnFetchColor( i );
            rgb[0] = color.redF();
            rgb[1] = color.greenF();
            rgb[2] = color.blueF();
            //WARNING: avoid using different transparency levels
            //         weird transparency effects have been observed.
            //         set the same alpha for all colors in the table
            lut->SetTableValue(i, rgb[0], rgb[1], rgb[2], 1.0);
            lut->SetAnnotation(i, QString::number(i).toStdString() );
        } else {
            lut->SetTableValue(i, 0.0, 0.0, 0.0, 1.0);
            lut->SetAnnotation(i, "UNKNOWN CATEGORY" );
        }
    }
    lut->IndexedLookupOn();
    lut->SetNanColor( 0.0, 0.0, 0.0, 1.0 ); //ilegal color codes are rendered as 100% transparent.
    lut->Build();

    return lut;
}

vtkSmartPointer<vtkLookupTable> Util::getClassicRainbow(double min, double max)
{
    size_t tableSize = 32;

    //create a color interpolator object
    vtkSmartPointer<vtkColorTransferFunction> ctf =
            vtkSmartPointer<vtkColorTransferFunction>::New();
    ctf->SetColorSpaceToRGB();
    ctf->AddRGBPoint(0.00, 0.000, 0.000, 1.000);
    ctf->AddRGBPoint(0.25, 0.000, 1.000, 1.000);
    ctf->AddRGBPoint(0.50, 0.000, 1.000, 0.000);
    ctf->AddRGBPoint(0.75, 1.000, 1.000, 0.000);
    ctf->AddRGBPoint(1.00, 1.000, 0.000, 0.000);

    //create the color table object
    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetTableRange(min, max);
    lut->SetNumberOfTableValues(tableSize);
    for(size_t i = 0; i < tableSize; ++i)
    {
        double *rgb;
        rgb = ctf->GetColor(static_cast<double>(i)/tableSize);
        lut->SetTableValue(i, rgb[0], rgb[1], rgb[2]);
    }
    lut->SetRampToLinear();
    lut->Build();

    return lut;
}

vtkSmartPointer<vtkLookupTable> Util::getSeismic(double min, double max)
{
    size_t tableSize = 32;

    //create a color interpolator object
    vtkSmartPointer<vtkColorTransferFunction> ctf =
            vtkSmartPointer<vtkColorTransferFunction>::New();
    ctf->SetColorSpaceToRGB();
    ctf->AddRGBPoint(0.00, 0.000, 0.000, 1.000);
    ctf->AddRGBPoint(0.50, 1.000, 1.000, 1.000);
    ctf->AddRGBPoint(1.00, 1.000, 0.000, 0.000);

    //create the color table object
    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetTableRange(min, max);
    lut->SetNumberOfTableValues(tableSize);
    for(size_t i = 0; i < tableSize; ++i)
    {
        double *rgb;
        rgb = ctf->GetColor(static_cast<double>(i)/tableSize);
        lut->SetTableValue(i, rgb[0], rgb[1], rgb[2]);
    }
    lut->SetRampToLinear();
    lut->Build();

    return lut;
}
