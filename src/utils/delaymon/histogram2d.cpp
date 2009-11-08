#include "histogram2d.h"
#include <assert.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm> 

using namespace delaymon;
using namespace std;

Histogram2d::Histogram2d( int start, int end, int stride, const std::string& fileprefix ) :
        start_(start),
        end_(end),
        stride_(stride),
        binsPerDim_(0),
        file_(0)
{
    assert( stride>0 && "stride parameter in historgram must be positive" );
    assert( start_<end_ && "start must be smaller than end" );
 
    binsPerDim_ = (int)floor( (end_ - start_) / stride_ );
    assert( binsPerDim_>0 && "histogram must have at least 1 bin" );
    counters_.resize(binsPerDim_);
    vector<int> values(binsPerDim_);
    std::fill( values.begin(), values.end(), 0 );
    std::fill( counters_.begin(), counters_.end(), values );
    cout<<"Configured histogram with " << binsPerDim_ << " bins"<<endl;

    string edges_filename = fileprefix + "2d_edges.txt";
    file_ = new ofstream( edges_filename.c_str() );
    if ( !file_->is_open() ) {
        cout<<"Histogram2d: Could not create edges file " << edges_filename << endl;
        assert( !"Could not create edges output file" );
    }
    else {
        cout<<"Histogram2d: Created edges output file " << edges_filename << endl;
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

    string bins_filename = fileprefix + "2d_bins.txt";
    file_ = new ofstream( bins_filename.c_str() );
    if ( !file_->is_open() ) {
        cout<<"Histogram2d: Could not create bins file " << bins_filename << endl;
        assert( !"Could not create bins output file" );
    }
    else {
        cout<<"Histogram2d: Created bins output file " << bins_filename << endl;
    }
}

Histogram2d::~Histogram2d()
{
    string output = toString();

    cout << output << endl;

    (*file_) << output << endl;
    file_->close();
    delete file_;
}

void 
Histogram2d::addValue( int valueX, int valueY ) 
{   
    int indexX=0;
    int indexY=0;
    bin( valueX, valueY, indexX, indexY );
    assert( indexX >= 0 );
    assert( indexY >= 0 );
    assert( indexX<(int)counters_.size() && "bin index X exceeds histogram size" );
    assert( indexY<(int)counters_[indexX].size() && "bin index Y exceeds histogram size" );
    counters_[indexX][indexY]++;
}

std::string 
Histogram2d::toString() const
{
    stringstream ss;
    
    for ( size_t x=0; x<counters_.size(); ++x )
        for ( size_t y=0; y<counters_[x].size(); ++y )
            ss << counters_[x][y] << " ";
    return ss.str();
}

vector<int> 
Histogram2d::edges() const
{
    vector<int> binEdges;
    for ( size_t i=0; i<counters_.size()+1; ++i )
        binEdges.push_back( start_ + i*stride_ );

    return binEdges;
}

void 
Histogram2d::bin( int valueX, int valueY, int &indexX, int &indexY )
{
    
    if ( valueX < start_ )
        indexX = 0;
    else if ( valueX >= end_ )
        indexX = binsPerDim_-1;
    else 
        indexX = (int)floor( (valueX - start_) / stride_ );

    if ( valueY < start_ )
        indexY = 0;
    else if ( valueY >= end_ )
        indexY = binsPerDim_-1;
    else 
        indexY = (int)floor( (valueY - start_) / stride_ );
}
