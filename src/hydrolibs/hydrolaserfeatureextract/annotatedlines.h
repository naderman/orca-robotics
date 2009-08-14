#ifndef HYDROLFEXTRACT_ANNOTATEDLINES_H
#define HYDROLFEXTRACT_ANNOTATEDLINES_H

#include <hydrolaserfeatureextract/lines.h>

namespace hydrolfextract {

enum EndpointVisibility {
    NotVisible,
    VisibleInternalLineJunction,
    VisibleExternalLineJunction,
    VisibleFreeStandingLine,
};
std::string toString( EndpointVisibility e );
inline std::ostream &operator<<( std::ostream &s, const EndpointVisibility &e )
{ return s << toString(e); }
inline bool isVisible( EndpointVisibility e )
{ return e != NotVisible; }

class AnnotatedCluster {
public:

    AnnotatedCluster( const Cluster &cluster );

    const Cluster &cluster() const { return cluster_; }

    const std::vector<Point> &points() const { return cluster_.points(); }
    unsigned int numLineSegments() const { return cluster_.numLineSegments(); }
    const std::vector<LineSegment> &lines() const { return cluster_.lines(); }

    const Point &firstPointInLine( int lineI ) const { return cluster_.firstPointInLine(lineI); }
    const Point &lastPointInLine( int lineI ) const { return cluster_.lastPointInLine(lineI); }

    double lineLength( int lineI ) const { return cluster_.lineLength(lineI); }

    EndpointVisibility startVisibility( int lineI ) const { return notes_[lineI].startVisibility; }
    EndpointVisibility endVisibility( int lineI ) const { return notes_[lineI].endVisibility; }
    double maxLineFitError( int lineI ) const { return notes_[lineI].maxLineFitError; }
    double lineFitRhoSd( int lineI ) const { return notes_[lineI].lineFitRhoSd; }
    double lineFitAlphaSd( int lineI ) const { return notes_[lineI].lineFitAlphaSd; }

    void setStartVisibility( int lineI, EndpointVisibility v ) { notes_[lineI].startVisibility = v; }
    void setEndVisibility( int lineI, EndpointVisibility v ) { notes_[lineI].endVisibility = v; }
    void setMaxLineFitError( int lineI, double maxLineFitError ) { notes_[lineI].maxLineFitError = maxLineFitError; }
    void setLineFitUncertainty( int lineI, double rhoSd, double alphaSd )
        { notes_[lineI].lineFitRhoSd = rhoSd; notes_[lineI].lineFitAlphaSd = alphaSd; }

private:

    struct LineNotes {
        // The most distance by which any single point deviates from the best-fit line
        double maxLineFitError;

        // The uncertainty in the line estimate due to line-fitting issues
        double lineFitRhoSd;
        double lineFitAlphaSd;

        EndpointVisibility startVisibility;
        EndpointVisibility endVisibility;
    };

    Cluster cluster_;
    // One per line segment
    std::vector<LineNotes> notes_;
};
std::string toString( const AnnotatedCluster &a );
inline std::ostream &operator<<( std::ostream &s, const AnnotatedCluster &a )
{ return s << toString(a); }

class AnnotatedLines {
public:
    AnnotatedLines( const Lines &lines );

    unsigned int numClusters() const { return annotatedClusters_.size(); }
    const std::vector<AnnotatedCluster> &clusters() const { return annotatedClusters_; }
    std::vector<AnnotatedCluster> &clusters() { return annotatedClusters_; }
    
private:

    std::vector<AnnotatedCluster> annotatedClusters_;
};
std::string toString( const AnnotatedLines &a );
inline std::ostream &operator<<( std::ostream &s, const AnnotatedLines &a )
{ return s << toString(a); }

AnnotatedLines
getAnnotatedLines( const Lines &lines,
                   const std::vector<float> &ranges,
                   double minLineLengthForVisibleEndpoints );

}

#endif
