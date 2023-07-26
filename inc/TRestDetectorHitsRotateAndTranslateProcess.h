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

#ifndef RestCore_TRestDetectorHitsRotateAndTranslateProcess
#define RestCore_TRestDetectorHitsRotateAndTranslateProcess

#include "TRestLegacyProcess.h"

//! A process to identify signal and remove baseline noise from a TRestRawSignalEvent.
class TRestDetectorHitsRotateAndTranslateProcess : public TRestLegacyProcess {
   private:
    /// Shift in X-axis
    Double_t fDeltaX;  //<

    /// Shift in Y-axis
    Double_t fDeltaY;  //<

    /// Shift in Z-axis
    Double_t fDeltaZ;  //<

    /// Rotation angle around Z-axis
    Double_t fAlpha;  //<

    /// Rotation angle around Y-axis
    Double_t fBeta;  //<

    /// Rotation angle around X-axis
    Double_t fGamma;  //<

   public:
    inline Double_t GetDeltaX() const { return fDeltaX; }
    inline Double_t GetDeltaY() const { return fDeltaY; }
    inline Double_t GetDeltaZ() const { return fDeltaZ; }

    inline Double_t GetAlpha() const { return fAlpha; }
    inline Double_t GetBeta() const { return fBeta; }
    inline Double_t GetGamma() const { return fGamma; }
    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() override {
        BeginPrintProcess();

        RESTMetadata << " Delta x : " << fDeltaX << RESTendl;
        RESTMetadata << " Delta y : " << fDeltaY << RESTendl;
        RESTMetadata << " Delta z : " << fDeltaZ << RESTendl;
        RESTMetadata << " Alpha : " << fAlpha << RESTendl;
        RESTMetadata << " Beta : " << fBeta << RESTendl;
        RESTMetadata << " Gamma : " << fGamma << RESTendl;

        EndPrintProcess();
    }

    TRestDetectorHitsRotateAndTranslateProcess() {
        RESTWarning << "Creating legacy process TRestDetectorHitsRotateAndTranslateProcess" << RESTendl;
        RESTWarning << "This process is now implemented under TRestRawToDetectorSignalProcess" << RESTendl;
    }
    TRestDetectorHitsRotateAndTranslateProcess(char* cfgFileName) {
        RESTWarning << "Creating legacy process TRestDetectorHitsRotateAndTranslateProcess" << RESTendl;
        RESTWarning << "This process is now implemented under TRestRawToDetectorSignalProcess" << RESTendl;
    }

    ClassDefOverride(TRestDetectorHitsRotateAndTranslateProcess, 4);
};
#endif
