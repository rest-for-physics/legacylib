/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef RestCore_TRestSignalZeroSuppresionProcess
#define RestCore_TRestSignalZeroSuppresionProcess

//#include <TRestGas.h>
//#include <TRestReadout.h>

#include "TRestLegacyProcess.h"

//! A process to identify signal and remove baseline noise from a TRestRawSignalEvent.
class TRestSignalZeroSuppresionProcess : public TRestEventProcess {
   private:
    /// The ADC range used for baseline offset definition
    TVector2 fBaseLineRange;

    /// The ADC range used for integral definition and signal identification
    TVector2 fIntegralRange;

    /// Number of sigmas over baseline fluctuation to accept a point is over threshold.
    Double_t fPointThreshold;

    /// A threshold parameter to accept or reject a pre-identified signal. See process description.
    Double_t fSignalThreshold;

    /// Number of consecutive points over threshold required to accept a signal.
    Int_t fPointsOverThreshold;

    Int_t fNPointsFlatThreshold;

    /// A parameter to determine if baseline correction has been applied by a previous process, not used
    bool fBaseLineCorrection;

    /// A parameter to determine whether to cut empty events
    bool fCutEmptyEvents;

    /// The ADC sampling used to transform ADC units to physical time in the output TRestSignalEvent. Given in
    /// us.
    Double_t fSampling;

   public:
    /// It prints out the process parameters stored in the RESTMetadata structure
    void PrintMetadata() {
        BeginPrintProcess();

        RESTMetadata << "Base line range definition : ( " << fBaseLineRange.X() << " , " << fBaseLineRange.Y()
                     << " ) " << RESTendl;
        RESTMetadata << "Integral range : ( " << fIntegralRange.X() << " , " << fIntegralRange.Y() << " ) "
                     << RESTendl;
        RESTMetadata << "Point Threshold : " << fPointThreshold << " sigmas" << RESTendl;
        RESTMetadata << "Signal threshold : " << fSignalThreshold << " sigmas" << RESTendl;
        RESTMetadata << "Number of points over threshold : " << fPointsOverThreshold << RESTendl;
        RESTMetadata << "Sampling rate : " << 1. / fSampling << " MHz" << RESTendl;
        RESTMetadata << "Max Number of points of flat signal tail : " << fNPointsFlatThreshold << RESTendl;
        RESTMetadata << "Cut empty events : " << (fCutEmptyEvents ? "true" : "false") << RESTendl;

        if (fBaseLineCorrection)
            RESTMetadata << "BaseLine correction is enabled for TRestRawSignalAnalysisProcess" << RESTendl;

        EndPrintProcess();
    }

    LegacyProcessDef(TRestSignalZeroSuppresionProcess, TRestRawToDetectorSignalProcess, 4);
};
#endif
