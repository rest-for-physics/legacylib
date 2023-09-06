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

#ifndef RestCore_TRestLegacyProcess
#define RestCore_TRestLegacyProcess

#include "TRestEventProcess.h"

//! Base class for legacy process
class TRestLegacyProcess : public TRestEventProcess {
   public:
    RESTValue GetInputEvent() const final { return RESTValue((TRestEvent*)nullptr); }
    RESTValue GetOutputEvent() const final { return RESTValue((TRestEvent*)nullptr); }

    void InitProcess() final{};
    TRestEvent* ProcessEvent(TRestEvent* eventInput) final {
        RESTError << "You are trying to execute a legacy process" << RESTendl;
        RESTError << "This is not allow, this class is kept for backward compatibility" << RESTendl;
        exit(1);
        return nullptr;
    }
    void EndProcess() final{};

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() override {}

    /// Returns the name of this process
    const char* GetProcessName() const final { return "legacyProcess"; }

    TRestLegacyProcess() {}
    TRestLegacyProcess(char* cfgFileName) {}
    ~TRestLegacyProcess() {}

    ClassDefOverride(TRestLegacyProcess, 0);
};
#endif
