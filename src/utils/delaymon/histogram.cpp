#include "histogram.h"
#include <assert.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace delaymon;
using namespace std;

Histogram::Histogram( int start, int end, int stride, const std::string& fileprefix ) :
        start_(start),
        end_(end),
        stride_(stride),
        file_(0)
{
    assert( stride>0 && "stride parameter in historgram must be positive" );
    assert( start_<end_ && "start must be smaller than end" );

    int binCount = (int)floor( (end_ - start_) / stride_ );
    assert( binCount>0 && "histogram must have at least 1 bin" );
    for ( int i=0; i<binCount; ++i )
        counters_.push_back( 0 );
    cout<<"Configured histogram with " << binCount << " bins"<<endl;

    string edges_filename = fileprefix + "_edges.txt";
    file_ = new ofstream( edges_filename.c_str() );
    if ( !file_->is_open() ) {
        cout<<"Histogram: Could not create edges file " << edges_filename << endl;
        assert( !"Could not create edges output file" );
    }
    else {
        cout<<"Histogram: Created edges output file " << edges_filename << endl;
    }

    vector<int> binEdges = edges();
    stringstream ss;
    for ( size_t i=0; i<binEdges.size(); ++i )
        ss << binEdges[i] << " ";
    cout<<"Bin edges: "<<endl;
    cout<<ss.str()<<endl;
    (*file_) << ss.str() << endl;
    file_->close();
    delete file_;

    string bins_filename = fileprefix + "_bins.txt";
    file_ = new ofstream( bins_filename.c_str() );
    if ( !file_->is_open() ) {
        cout<<"Histogram: Could not create bins file " << bins_filename << endl;
        assert( !"Could not create bins output file" );
    }
    else {
        cout<<"Histogram: Created bins output file " << bins_filename << endl;
    }
}

Histogram::~Histogram()
{
    string output = toString();

    cout << output << endl;

    (*file_) << output << endl;
    file_->close();
    delete file_;
}

void 
Histogram::addValue( int value ) 
{   
    const int binIndex = bin( value );
    assert( binIndex<(int)counters_.size() && "bin index exceeds histogram size" );
    counters_[ binIndex ]++;
}

std::string 
Histogram::toString() const
{
    stringstream ss;
    for ( size_t i=0; i<counters_.size(); ++i )
        ss << counters_[i] << " ";
    return ss.str();
}

vector<int> 
Histogram::edges() const
{
    vector<int> binEdges;
    for ( size_t i=0; i<counters_.size()+1; ++i )
        binEdges.push_back( start_ + i*stride_ );

    return binEdges;
}

int 
Histogram::bin( int value )
{
    if ( value < start_ )
        return 0;

    if ( value >= end_ )
        return counters_.size()-1;
    
    return (int)floor( (value - start_) / stride_ );
}
