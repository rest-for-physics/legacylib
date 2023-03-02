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

#ifndef RestCore_TRestRawZeroSuppresionProcess
#define RestCore_TRestRawZeroSuppresionProcess

#include "TRestLegacyProcess.h"

//! A process to identify signal and remove baseline noise from a TRestRawSignalEvent.
class TRestRawZeroSuppresionProcess : public TRestLegacyProcess {
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
    Int_t fNPointsOverThreshold;

    Int_t fNPointsFlatThreshold;

    /// A parameter to determine if baseline correction has been applied by a previous process
    bool fBaseLineCorrection;

    /// The ADC sampling used to transform ADC units to physical time in the output TRestDetectorSignalEvent.
    /// Given in us.
    Double_t fSampling;

   public:
    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() override {
        BeginPrintProcess();
        RESTMetadata << "Base line range definition : ( " << fBaseLineRange.X() << " , " << fBaseLineRange.Y()
                     << " ) " << RESTendl;
        RESTMetadata << "Integral range : ( " << fIntegralRange.X() << " , " << fIntegralRange.Y() << " ) "
                     << RESTendl;
        RESTMetadata << "Point Threshold : " << fPointThreshold << " sigmas" << RESTendl;
        RESTMetadata << "Signal threshold : " << fSignalThreshold << " sigmas" << RESTendl;
        RESTMetadata << "Number of points over threshold : " << fNPointsOverThreshold << RESTendl;
        RESTMetadata << "Sampling rate : " << 1. / fSampling << " MHz" << RESTendl;
        RESTMetadata << "Max Number of points of flat signal tail : " << fNPointsFlatThreshold << RESTendl;
        if (fBaseLineCorrection)
            RESTMetadata << "BaseLine correction is enabled for TRestRawSignalAnalysisProcess" << RESTendl;

        EndPrintProcess();
    }

    TRestRawZeroSuppresionProcess() {
        RESTWarning << "Creating legacy process TRestRawZeroSuppresionProcess" << RESTendl;
        RESTWarning << "This process is now implemented under TRestRawToDetectorSignalProcess" << RESTendl;
    }

    TRestRawZeroSuppresionProcess(char* cfgFileName) {
        RESTWarning << "Creating legacy process TRestRawZeroSuppresionProcess" << RESTendl;
        RESTWarning << "This process is now implemented under TRestRawToDetectorSignalProcess" << RESTendl;
    }

    ClassDefOverride(TRestRawZeroSuppresionProcess, 4);
};
#endif
