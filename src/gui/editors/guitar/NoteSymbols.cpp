#include "NoteSymbols.h"
#include "misc/Debug.h"

namespace Rosegarden
{

namespace Guitar
{
NoteSymbols::posPair
NoteSymbols::getX ( int imgWidth, unsigned int stringNb, unsigned int nbOfStrings )
{
    /*
            std::cout << "NoteSymbols::getX - input values" << std::endl
            << "  position: " << position << std::endl
            << "  string #: " << string_num << std::endl
            << "  scale:    " << scale << std::endl;
    */
    unsigned int lBorder = getLeftBorder( imgWidth );
    unsigned int fretboardWidth = getFretboardWidth( imgWidth );
    unsigned int columnWidth = fretboardWidth / nbOfStrings;
    return std::make_pair( ( stringNb * columnWidth + lBorder ), columnWidth );
}

NoteSymbols::posPair
NoteSymbols::getY ( int imgHeight, unsigned int fretNb, unsigned int nbOfFrets )
{
    /*
            std::cout << "NoteSymbols::getY - input values" << std::endl
            << "  position: " << fret_pos << std::endl
            << "  max frets:   " << maxFretNum << std::endl
            << "  scale:    " << scale << std::endl;
    */
    unsigned int tBorder = getTopBorder( imgHeight );
    unsigned int fretboardHeight = getFretboardHeight( imgHeight );
    unsigned int rowHeight = fretboardHeight / nbOfFrets;
    return std::make_pair( ( ( fretNb * rowHeight ) + tBorder ), rowHeight );
}

void
NoteSymbols::drawMuteSymbol ( QPainter* p,
                              unsigned int position )
{
    QRect v = p->viewport();

    posPair x_pos = getX ( v.width(), position, m_nbOfStrings );
    unsigned int y_pos = getTopBorder( v.height() ) / 2;
    double columnWidth = x_pos.second;
    unsigned int width = static_cast<unsigned int>( columnWidth * 0.7 );
    unsigned int height = static_cast<unsigned int>( columnWidth * 0.7 );

    //std::cout << "NoteSymbols::drawMuteSymbol - drawing Mute symbol at string #" << position
    //<< std::endl;

    p->drawLine ( x_pos.first - ( width / 2 ),
                  y_pos - ( height / 2 ),
                  ( x_pos.first + ( width / 2 ) ),
                  y_pos + ( height / 2 ) );

    p->drawLine( x_pos.first + ( width / 2 ),
                 y_pos - ( height / 2 ),
                 ( x_pos.first - ( width / 2 ) ),
                 y_pos + ( height / 2 ) );
}

void
NoteSymbols::drawOpenSymbol ( QPainter* p,
                              unsigned int position )
{
    QRect v = p->viewport();
    posPair x_pos = getX ( v.width(), position, m_nbOfStrings );
    unsigned int y_pos = getTopBorder( v.height() ) / 2;
    double columnWidth = x_pos.second;
    unsigned int radius = static_cast<unsigned int>( columnWidth * 0.7 );

    //std::cout << "NoteSymbols::drawOpenSymbol - drawing Open symbol at string #" << position
    //<< std::endl;

    p->setBrush( Qt::NoBrush );
    p->drawEllipse( x_pos.first - ( radius / 2 ),
                    y_pos - ( radius / 2 ),
                    radius,
                    radius );
}

void
NoteSymbols::drawNoteSymbol ( QPainter* p,
                              unsigned int stringNb,
                              int fretNb,
                              bool transient )
{
    //std::cout << "NoteSymbols::drawNoteSymbol - string: " << position << ", fret:" << fret
    //<< std::endl;

    QRect v = p->viewport();
    posPair x_pos = getX ( v.width(), stringNb, m_nbOfStrings );
    posPair y_pos = getY ( v.height(), fretNb, m_nbOfFrets );
    double columnWidth = x_pos.second;
    unsigned int radius;

    if (transient) {
        radius =  static_cast<unsigned int>( columnWidth /* * 0.9 */ );
        p->setBrush( Qt::NoBrush );
    } else {
        radius =  static_cast<unsigned int>( columnWidth * 0.7 );
        p->setBrush( Qt::SolidPattern );
    }

    int x = x_pos.first - ( radius / 2 ),
        y = y_pos.first + ( (y_pos.second - radius) / 2) + TOP_FRETBOARD_MARGIN; 

//    RG_DEBUG << "NoteSymbols::drawNoteSymbol : rect = " << QRect(x,y, radius, radius) << endl;

    p->drawEllipse( x,
                    y,
                    radius,
                    radius );
                    
//    p->save();
//    p->setPen(Qt::red);
//    p->drawRect( x, y, radius, radius );
//    p->restore();
}

void
NoteSymbols::drawBarreSymbol ( QPainter* p,
                               int fretNb,
                               unsigned int start,
                               unsigned int end )
{

    //std::cout << "NoteSymbols::drawBarreSymbol - start: " << start << ", end:" << end << std::endl;

    drawNoteSymbol ( p, start, fretNb );

    if ( ( end - start ) >= 1 ) {
        QRect v = p->viewport();
        posPair startXPos = getX ( v.width(), start, m_nbOfStrings );
        posPair endXPos = getX ( v.width(), end, m_nbOfStrings );
        posPair y_pos = getY ( v.height(), fretNb, m_nbOfFrets );
        double columnWidth = startXPos.second;
        unsigned int thickness = static_cast<unsigned int>( columnWidth * 0.7 );

        p->drawRect( startXPos.first,
                     y_pos.first + ( y_pos.second / 4 ) + TOP_FRETBOARD_MARGIN,
                     endXPos.first - startXPos.first,
                     thickness );
    }

    drawNoteSymbol ( p, end, fretNb );
}

void
NoteSymbols::drawFretNumber ( QPainter* p,
                              unsigned int fret_num )
{
    if ( fret_num > 1 ) {
        QRect v = p->viewport();
        unsigned int imgWidth = v.width();
        unsigned int imgHeight = v.height();

        QString tmp;
        tmp.setNum( fret_num );

        // Use NoteSymbols to grab X and Y for first fret
        posPair y_pos = getY( imgHeight, 0, m_nbOfFrets );

        p->drawText( getLeftBorder( imgWidth ) / 4,
                     y_pos.first + ( y_pos.second / 2 ),
                     tmp );
    }
}

void
NoteSymbols::drawFrets ( QPainter* p )
{
    /*
            std::cout << "NoteSymbols::drawFretHorizontalLines" << std::endl
            << "  scale: " << scale << std::endl
            << "  frets: " << fretsDisplayed << std::endl
            << "  max string: " << maxStringNum << std::endl;
    */

    QRect v = p->viewport();
    unsigned int imgWidth = v.width();
    unsigned int imgHeight = v.height();
    //unsigned int endXPos = getFretboardWidth(imgWidth) + getLeftBorder(imgWidth);
    posPair endXPos = getX ( imgWidth, m_nbOfStrings - 1, m_nbOfStrings );

    unsigned int yFretboard = getFretboardHeight( imgHeight );
    unsigned int rowHeight = yFretboard / m_nbOfFrets;

    QPen pen;
    pen.setWidth(FRET_PEN_WIDTH);  
    p->save();
    p->setPen(pen);
    unsigned int y_pos = (getY ( imgHeight, 0, m_nbOfFrets )).first + TOP_FRETBOARD_MARGIN;
    
    NOTATION_DEBUG << "NoteSymbols::drawFrets : " << m_nbOfFrets << endl;
    
    // Horizontal lines
    for ( unsigned int i = 0; i <= m_nbOfFrets; ++i ) {

        /* This code borrowed from KGuitar 0.5 */
        p->drawLine( getLeftBorder( imgWidth ),
                     y_pos,
                     endXPos.first,
                     y_pos);
        NOTATION_DEBUG << "NoteSymbols::drawFrets : " << QPoint(getLeftBorder(imgWidth), y_pos)
                       << " to " << QPoint(endXPos.first, y_pos) << endl;
                     

       y_pos += rowHeight;
    }

    p->restore();

}

void
NoteSymbols::drawStrings ( QPainter* p )
{
    // Vertical lines
    QRect v = p->viewport();
    int imgHeight = v.height();
    int imgWidth = v.width();

    unsigned int startPos = getTopBorder( imgHeight ) + TOP_FRETBOARD_MARGIN;
    unsigned int endPos = (getY ( imgHeight, m_nbOfFrets, m_nbOfFrets )).first + TOP_FRETBOARD_MARGIN;

    unsigned int fretboard = getFretboardWidth( imgWidth );
    unsigned int columnWidth = fretboard / m_nbOfStrings;

    unsigned int x_pos = (getX ( imgWidth, 0, m_nbOfStrings )).first;

    QPen pen;
    pen.setWidth(STRING_PEN_WIDTH);  
    p->save();
    p->setPen(pen);

    for ( unsigned int i = 0; i < m_nbOfStrings; ++i ) {

        /* This code borrowed from KGuitar 0.5 */
        p->drawLine( x_pos,
                     startPos,
                     x_pos,
                     endPos );
                     
       x_pos += columnWidth;
    }

    p->restore();
    
}

QRect NoteSymbols::getTransientNoteSymbolRect(QSize fretboardSize,
                                              unsigned int stringNb,
                                              int fretNb)
{
    posPair x_pos = getX ( fretboardSize.width(), stringNb, m_nbOfStrings );
    posPair y_pos = getY ( fretboardSize.height(), fretNb, m_nbOfFrets );
    double columnWidth = x_pos.second;
    unsigned int radius =  static_cast<unsigned int>( columnWidth /* * 0.9 */ );

    int x = x_pos.first - ( radius / 2 ),
        y = y_pos.first + ( (y_pos.second - radius) / 2) + TOP_FRETBOARD_MARGIN; 

    return QRect(x, y, radius, radius);
}

unsigned int
NoteSymbols::getTopBorder ( unsigned int imgHeight )
{
    return static_cast<unsigned int>( TOP_BORDER_PERCENTAGE * imgHeight );
}

unsigned int
NoteSymbols::getBottomBorder ( unsigned int imgHeight )
{
    return static_cast<unsigned int>( imgHeight * BOTTOM_BORDER_PERCENTAGE );
}

unsigned int
NoteSymbols::getLeftBorder ( unsigned int imgWidth )
{
    unsigned int left = static_cast<unsigned int>( imgWidth * LEFT_BORDER_PERCENTAGE );
    if ( left < 15 ) {
        left = 15;
    }
    return left;
}

unsigned int
NoteSymbols::getRightBorder ( unsigned int imgWidth )
{
    return static_cast<unsigned int>( imgWidth * RIGHT_BORDER_PERCENTAGE );
}

unsigned int
NoteSymbols::getFretboardWidth ( int imgWidth )
{
    return static_cast<unsigned int>( imgWidth * FRETBOARD_WIDTH_PERCENTAGE );
}

unsigned int
NoteSymbols::getFretboardHeight ( int imgHeight )
{
    return static_cast<unsigned int>( imgHeight * FRETBOARD_HEIGHT_PERCENTAGE );
}

std::pair<bool, unsigned int>
NoteSymbols::getStringNumber ( int imgWidth,
                               unsigned int x_pos,
                               unsigned int maxStringNum )
{
    /*
        std::cout << "NoteSymbols::getNumberOfStrings - input values" << std::endl
        << "  X position: " << x_pos << std::endl
        << "  string #: " << maxStringNum << std::endl
        << "  image width:    " << imgWidth << std::endl;
    */
    bool valueOk = false;

    posPair xPairPos;
    unsigned int min = 0;
    unsigned int max = 0;
    unsigned int result = 0;

    for ( unsigned int i = 0; i < maxStringNum; ++i ) {
        xPairPos = getX ( imgWidth, i, maxStringNum );

        // If the counter equals zero then we are at the first
        // string to the left
        if ( i == 0 ) {
            // Add 10 pixel buffer to range comparison
            min = xPairPos.first - 10;
        } else {
            min = xPairPos.first - xPairPos.second / 2;
        }

        // If the counter equals the maxString number -1 then we are at the last
        // string to the right
        if ( i == ( maxStringNum - 1 ) ) {
            // Add 10 pixel buffer to range comparison
            max = xPairPos.first + 10;
        } else {
            max = xPairPos.first + xPairPos.second / 2;
        }

        if ( ( x_pos >= min ) && ( x_pos <= max ) ) {
            result = i;
            valueOk = true;
            break;
        }
    }

    //std::cout << "NoteSymbols::getNumberOfStrings - string: #" << result << std::endl;
    return std::make_pair( valueOk, result );
}

std::pair<bool, unsigned int>
NoteSymbols::getFretNumber ( int imgHeight,
                             unsigned int y_pos,
                             unsigned int maxFretNum )
{
    /*
        std::cout << "NoteSymbols::getNumberOfFrets - input values" << std::endl
        << "  Y position: " << y_pos << std::endl
        << "  max frets:   " << maxFretNum << std::endl
        << "  image height:    " << imgHeight << std::endl;
    */

    bool valueOk = false;
    unsigned int tBorder = getTopBorder( imgHeight );
    unsigned int result = 0;

    if ( y_pos < tBorder ) {
        // User pressing above the fretboard to mark line muted or opened
        valueOk = true;
    } else {
        typedef std::pair<unsigned int, unsigned int> RangePair;

        posPair min_pos;
        posPair max_pos;

        for ( unsigned int i = 0; i < maxFretNum; ++i ) {
            min_pos = getY ( imgHeight, i, maxFretNum );
            max_pos = getY ( imgHeight, i + 1, maxFretNum );

            if ( ( y_pos >= min_pos.first ) && y_pos <= max_pos.first - 1 ) {
                result = i;
                valueOk = true;
                break;
            }
        }
    }
    //    std::cout << "  fret #: " << result << std::endl;
    return std::make_pair( valueOk, result );
}

float const NoteSymbols::LEFT_BORDER_PERCENTAGE = 0.1;
float const NoteSymbols::RIGHT_BORDER_PERCENTAGE = 0.1;
float const NoteSymbols::FRETBOARD_WIDTH_PERCENTAGE = 0.8;
float const NoteSymbols::TOP_BORDER_PERCENTAGE = 0.1;
float const NoteSymbols::BOTTOM_BORDER_PERCENTAGE = 0.1;
float const NoteSymbols::FRETBOARD_HEIGHT_PERCENTAGE = 0.8;
int   const NoteSymbols::TOP_FRETBOARD_MARGIN = 5;
int   const NoteSymbols::FRET_PEN_WIDTH = 2;
int   const NoteSymbols::STRING_PEN_WIDTH = 2;

} /* namespace Guitar */

}

