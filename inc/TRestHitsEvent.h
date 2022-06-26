
#ifndef TRestSoft_TRestHitsEvent
#define TRestSoft_TRestHitsEvent

#include <TGraph.h>

#include <iostream>

#include "TArrayI.h"
#include "TAxis.h"
#include "TGraph2D.h"
#include "TH2F.h"
#include "TMath.h"
#include "TObject.h"
#include "TRestEvent.h"
#include "TRestHits.h"
#include "TVector3.h"

//! An event data type that register a vector of TRestHits,
//! allowing us to save a 3-coordinate position and energy.
class TRestHitsEvent : public TRestEvent {
   private:
    /// An auxiliar TRestHits structure to register hits on XZ projection
    TRestHits* fXZHits;  //!
    /// An auxiliar TRestHits structure to register hits on YZ projection
    TRestHits* fYZHits;  //!

    /// An auxiliar TRestHits structure to register hits on XYZ projection
    TRestHits* fXYZHits;  //!

    /// Stores the minimum x-position value. It is initialized by SetBoundaries.
    Double_t fMinX;  //!
    /// Stores the maximum x-position value. It is initialized by SetBoundaries.
    Double_t fMaxX;  //!

    /// Stores the minimum y-position value. It is initialized by SetBoundaries.
    Double_t fMinY;  //!
    /// Stores the maximum y-position value. It is initialized by SetBoundaries.
    Double_t fMaxY;  //!

    /// Stores the minimum z-position value. It is initialized by SetBoundaries.
    Double_t fMinZ;  //!
    /// Stores the maximum z-position value. It is initialized by SetBoundaries.
    Double_t fMaxZ;  //!

   protected:
    // TODO These graphs should be placed in TRestHits?
    // (following similar GetGraph implementation in TRestSignal)

    /// An auxiliar TGraph pointer to visualize hits on XY-projection.
    TGraph* fXYHitGraph;  //!
    /// An auxiliar TGraph pointer to visualize hits on XZ-projection.
    TGraph* fXZHitGraph;  //!
    /// An auxiliar TGraph pointer to visualize hits on YZ-projection.
    TGraph* fYZHitGraph;  //!

    /// An auxiliar TH2F histogram to visualize hits on XY-projection.
    TH2F* fXYHisto;  //!
    /// An auxiliar TH2F histogram to visualize hits on YZ-projection.
    TH2F* fYZHisto;  //!
    /// An auxiliar TH2F histogram to visualize hits on XZ-projection.
    TH2F* fXZHisto;  //!

    TGraph2D* gxz = NULL;  //!
    TGraph2D* gyz = NULL;  //!

    /// An auxiliar TH1F histogram to visualize hits on X-projection.
    TH1F* fXHisto;  //!
    /// An auxiliar TH1F histogram to visualize hits on Y-projection.
    TH1F* fYHisto;  //!
    /// An auxiliar TH1F histogram to visualize hits on Z-projection.
    TH1F* fZHisto;  //!

    /// The hits structure that is is saved to disk.
    TRestHits fHits;  //

   public:
    void Initialize() {
        TRestEvent::Initialize();
        fHits.RemoveHits();
    }

    // Construtor
    TRestHitsEvent() {}
    // Destructor
    ~TRestHitsEvent() {}

    ClassDef(TRestHitsEvent, 2);
};
#endif
