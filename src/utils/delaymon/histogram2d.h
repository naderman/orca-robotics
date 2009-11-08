/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef HISTOGRAM2D_H
#define HISTOGRAM2D_H

#include <string>
#include <vector>

namespace delaymon
{

// not thread-safe
class Histogram2d
{
public:
    // start, end, stride in [ms]
    // stride must be positive
    // histogram must have at least 1 bin
    Histogram2d( int startEdge, int endEdge, int stride, const std::string& fileprefix );
    ~Histogram2d();

    void addValue( int valueX, int valueY );

    const std::vector<std::vector<int> > &counters() const { return counters_; };

    // -inf and +inf are not included even though those values are counted
    std::vector<int> edges() const;

    std::string toString() const;

private:

    
    // the bins are the same for both dimensions and defined as follows:
    //  [-inf, start)
    //  [start, start+stride)
    //  [start+strid, start+2*stride)
    //  ...
    //  [start+N-1*stride, start+N*stride)
    //  [start+N*stride, +inf)
    //
    // where (start+N*stride) <= end
    std::vector<std::vector<int> > counters_;
    int start_;
    int end_;
    int stride_;

    // bins per dimension (same for both dimensions)
    int binsPerDim_;
    
    void bin( int valueX, int valueY, int &indexX, int &indexY );

    std::ofstream *file_;
};

} // namespace

#endif
