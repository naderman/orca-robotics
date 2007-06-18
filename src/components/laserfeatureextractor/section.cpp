/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include "section.h"
#include "svd.h"
#include <sstream>

using namespace std;

namespace {
    const double LINEFIT_ERROR_THRESHOLD = 0.24;    // 0.10    0.24
}

Section::Section()
{
	//next = NULL;
	isNextCon_ = false;

	isALine_ = false;
	eigVectX_ = 0.0;
	eigVectY_ = 0.0;
	c_ = 0.0;
}


Section::~Section()
{
	// Nothing to delete
}

double 
Section::lineLength() const
{
    assert( isALine() );
    return hypotf( start().y()-end().y(), start().x()-end().x() );
}

void
Section::setEndPoints()
{
    // Project the end-points onto the line
    double off1 = eigVectX_*elements_.front().x() + eigVectY_*elements_.front().y()+ c_;
    double x1 = elements_.front().x() - off1*eigVectX_;
    double y1 = elements_.front().y() - off1*eigVectY_;
    start_.setXY( x1, y1 );

    double off2 = eigVectX_*elements_.back().x() + eigVectY_*elements_.back().y()+ c_;
    double x2 = elements_.back().x() - off2*eigVectX_;
    double y2 = elements_.back().y() - off2*eigVectY_;
    end_.setXY( x2, y2 );

    // Make sure the points are ordered correctly.
#ifndef NDEBUG    
    if ( end_.bearing()-start_.bearing() < 0 )
    {
        stringstream ss;
        ss<<"  setEndPoints(): Incorrect ordering of points: " << endl;
        ss<<"TRACE(section.cpp): startRB: " << start_.toStringRB() << endl;
        ss<<"TRACE(section.cpp): endRB  : " << end_.toStringRB() << endl;
        ss<<"TRACE(section.cpp): startXY: " << start_.toStringXY() << endl;
        ss<<"TRACE(section.cpp): endXY  : " << end_.toStringXY() << endl;
        ss<<"TRACE(section.cpp): firstRB: " << elements_.front().toStringRB() << endl;
        ss<<"TRACE(section.cpp): lastRB : " << elements_.back().toStringRB() << endl;
        ss<<"TRACE(section.cpp): firstXY: " << elements_.front().toStringXY() << endl;
        ss<<"TRACE(section.cpp): lastXY : " << elements_.back().toStringXY() << endl;
        //cout<<ss.str();
        //assert( false && "incorrect ordering of points!" );
        throw ss.str();
    }
#endif

    // sanity check...
    assert ( hypotf( start_.y()-elements().front().y(), start_.x()-elements().front().x() ) < 1.0 );
    assert ( hypotf( end_.y()-elements().back().y(),    end_.x()-elements().back().x() ) < 1.0 );
}

std::string
SectionEl::toStringXY() const
{
    stringstream ss;
    ss << "("<<x()<<", "<<y()<<")";
    return ss.str();
}
std::string
SectionEl::toStringRB() const
{
    stringstream ss;
    ss << "("<<range()<<", "<<bearing()*180.0/M_PI<<"deg)";
    return ss.str();
}

void 
extractSections( const std::vector<float> &ranges,
                 double angleStart,
                 double angleIncrement,
                 double maxRange,
                 double minPointsInLine,
                 double maxRangeDelta,
                 std::vector<Section> &sections )
{
    Section current;

    // Deal with first point
    if (ranges[0] < maxRange) 
    {
        double r = ranges[0];
        double b = angleStart;
        SectionEl pos(r, b);
        current.elements().push_back(pos);
        current.rangeBeforeStart() = -1;
    }

    // Loop through remaining points
    for (unsigned int i = 1; i < ranges.size(); i++) 
    {
        bool segmentBreak = ( ranges[i] >= maxRange ||
                              ranges[i-1] >= maxRange ||
                              fabs(ranges[i]-ranges[i-1]) > maxRangeDelta );
        if ( segmentBreak )
        {
            // Add the previous section if it's big enough, and start fresh
            if ( current.elements().size() >= minPointsInLine )
            {
                current.rangeAfterEnd() = ranges[i];
                sections.push_back(current);
            }
            current.elements().clear();
        }
        if ( ranges[i] < maxRange )
        {
            // Add this point to the current section
            double r = ranges[i];
            double b = angleStart + i*angleIncrement;
            current.elements().push_back( SectionEl(r,b) );
            if ( current.elements().size() == 1 )
                current.rangeBeforeStart() = ranges[i-1];
        }
    }

    // Finish final section
    if (current.elements().size() >= minPointsInLine)
    {
        // This section would have been added if there were a segment break.
        // Therefore it must contain the last return.
        current.rangeAfterEnd() = -1;
        sections.push_back(current);
    }
    
//     std::cout << "section.cpp: Found " << sections.size() << " sections" << std::endl;
//     //printSections( sections );
//     for ( unsigned int i=0; i < sections.size(); i++ )
//     {
//         cout << "  Sec "<<i<<": ("
//              <<sections[i].elements()[0].x()<<", "<<sections[i].elements()[0].y()<<") -> ("
//              <<sections[i].elements().back().x()<<", "<<sections[i].elements().back().y()<<")" << endl;
//     }
}

void 
fitLine( const std::vector<SectionEl> &elements,
         double &eigVectX, double &eigVectY, double &c )
{
    assert( elements.size() > 2 );
    // TODO: range-weight this calculation?

    int n = 2;
    int m = elements.size();

    // Find the centroid of the section..
    //std::cout << "Finding the centroid of the section" << std::endl;

    double centroidX=0, centroidY=0;
    for (int i = 0; i < m; i++) {
        centroidX += elements[i].x();
        centroidY += elements[i].y();
    }
    centroidX /= (double)m;
    centroidY /= (double)m;
  
    float *W = new float[n];

    float **A = new float*[m];
    for (int i = 0; i < m; i++) {
        A[i] = new float[n];
    }
    float **V = new float*[n];
    for (int i = 0; i < n; i++) {
        V[i] = new float[n];
    }

    for (int i = 0; i < m; i++) {
        A[i][0] = elements[i].x() - centroidX;
        A[i][1] = elements[i].y() - centroidY;
    }

    int ret = dsvd(A, m, n, W, V);
    assert( ret == 0 );

    int minIndex = 0;
    for (int i = 0; i < n; i++) {
        if (fabs(W[i]) < fabs(W[minIndex])) {
            minIndex = i;
        }
    }

    // the equation for the line is defined as:
    //
    //  vx x - vy y + c = 0
    //  vx x - vy y + (vy y0 + vx x0) = 0
    //
    //   where vx is the first component of the eigenvector
    //         vy is the second component of the eigencector
    //         c is the intercept
    eigVectX = V[0][minIndex];
    eigVectY = V[1][minIndex];
    c = -eigVectX*centroidX - eigVectY*centroidY;

    // Cleanup the arrays...
    delete[] W;
    for ( int i=0; i < m; i++ )
        delete[] A[i];
    delete[] A;
    for ( int i=0; i < n; i++ )
        delete[] V[i];
    delete[] V;
}

bool
findBreakPoint( const std::vector<SectionEl> &sectionElements,
                double eigVectX,
                double eigVectY,
                double c,
                double &maxDist,
                int    &pos )
{
    if ( sectionElements.size() < 3 )
    {
        // All points will be co-linear
        return false;
    }

    maxDist = -1;
    pos = -1;
    // Note: don't iterate over the end-points, since there's
    // no point breaking there.
    for ( unsigned int i=1; i < sectionElements.size()-1; i++ )
    {
        const SectionEl &e = sectionElements[i];

        // orthogonal distance from the line
        double orthDist = fabs( eigVectX*e.x() + eigVectY*e.y() + c );

        if ( orthDist > maxDist )
        {
            maxDist = orthDist;
            pos = i;
        }
    }

    assert( maxDist >= 0 );
    return true;
}

void 
breakAndFitLines( std::vector<Section> &sections, int minPointsInLine, double breakDistThreshold )
{
    std::vector<Section>::iterator itr = sections.begin();
    while (itr != sections.end())
    {
        if ( (int)(itr->elements().size()) < minPointsInLine ) 
        {
            ++itr;
            continue;
        }

        // try to fit a line to the section
        //double eigVectX, eigVectY, c;
        //fitLine( itr->elements(), eigVectX, eigVectY, c );
        // Instead: use the line defined by the endpoints
        double eigVectX =  (itr->elements().back().y()-itr->elements().front().y());
        double eigVectY = -(itr->elements().back().x()-itr->elements().front().x());
        double norm = hypotf(eigVectX,eigVectY);
        eigVectX /= norm;
        eigVectY /= norm;
        double c = -eigVectX*itr->elements().front().x() - eigVectY*itr->elements().front().y();
                

//         cout<<"TRACE(section.cpp): examining line from "<<itr->elements().front().toStringXY()<<" to "<<itr->elements().back().toStringXY() << "("<<itr->elements().size()<<" elements)" << endl;
//         cout<<"TRACE(section.cpp):               =from "<<itr->elements().front().toStringRB()<<" to "<<itr->elements().back().toStringRB() << endl;

        int    pos;
        double maxDist;
        if ( findBreakPoint( itr->elements(), eigVectX, eigVectY, c, maxDist, pos ) &&
             maxDist >= breakDistThreshold )
        {
            // We should break the line.
            //cout<<"TRACE(section.cpp): splitting at pos = " << pos << endl;
            
            // Split this section in two:
            //   a newSection just before the current one.
            //   the break point ends up in both segments.
            Section newSection;
            newSection.setIsNextCon( true );
            
            newSection.elements().insert( newSection.elements().begin(), 
                                          itr->elements().begin(), 
                                          itr->elements().begin() + pos+1);
            newSection.rangeBeforeStart() = itr->rangeBeforeStart();
            newSection.rangeAfterEnd() = itr->elements()[pos+1].range();

            itr->elements().erase( itr->elements().begin(), 
                                   itr->elements().begin() + pos );
            itr->rangeBeforeStart() = newSection.elements()[newSection.elements().size()-2].range();

            // insert the new section before the current one, and
            // point itr to it.
            itr = sections.insert(itr, newSection);
        }
        else
        {
            if ( (int)(itr->elements().size()) >= minPointsInLine )
            {
                // Fit a line properly now
                fitLine( itr->elements(), eigVectX, eigVectY, c );
                itr->setIsALine( eigVectX, eigVectY, c, maxDist );
            }
            ++itr;
        }
    }
}

void printSections( const std::vector<Section> &sections )
{
    int count = 0;

    cout << "Sections : ";
    for (std::vector<Section>::const_iterator itr = sections.begin(); itr != sections.end(); itr++) {
        count += itr->elements().size();
        cout << "{s:" << itr->elements().size();
        if (itr->isALine()) {
            cout << " L";
        }
        else {
            cout << " -";
        }
        if (itr->isNextCon()) {
            cout << "C";
        }
        else {
            cout << "-";
        }

        cout << "}  ";
    }
    cout << " - Count: " << count << endl;
}

