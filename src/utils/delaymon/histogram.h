/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <string>
#include <vector>

namespace delaymon
{

// not thread-safe
class Histogram
{
public:
    // start, end, stride in [ms]
    // stride must be positive
    // histogram must have at least 1 bin
    Histogram( int startEdge, int endEdge, int stride, const std::string& fileprefix );
    ~Histogram();

    void addValue( int );

    const std::vector<int>& counters() const { return counters_; };

    // -inf and +inf are not included even though those values are counted
    std::vector<int> edges() const;

    std::string toString() const;

private:
    // the bins are defined as follows:
    //  [-inf, start)
    //  [start, start+stride)
    //  [start+strid, start+2*stride)
    //  ...
    //  [start+N-1*stride, start+N*stride)
    //  [start+N*stride, +inf)
    //
    // where (start+N*stride) <= end
    std::vector<int> counters_;
    int start_;
    int end_;
    int stride_;

    int bin( int value );

    std::ofstream *file_;
};

} // namespace

#endif
