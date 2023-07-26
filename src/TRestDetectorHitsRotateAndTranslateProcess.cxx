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

//////////////////////////////////////////////////////////////////////////
/// IMPORTANT TRestDetectorHitsRotateAndTranslateProcess is a legacy class, it is just
/// kept for backward compatibility of the code. Do not attempt to create
/// any instance to a legacy process. The information below is kept to
/// have a reference of previous implementations.
///
/// You may use TRestDetectorHitsRotationProcess, TRestDetectorHitsTranslationProcess
/// or TRestDetectorHitsSpecularProcess in any combination to achieve the
/// same results as this process.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-March: Conception and implementation of signal zero suppression
/// process.
///               Javier Gracia
///
/// \class      TRestDetectorHitsRotateAndTranslateProcess
/// \author     Javier Gracia
///
/// <hr>
///

#include "TRestDetectorHitsRotateAndTranslateProcess.h"

ClassImp(TRestDetectorHitsRotateAndTranslateProcess);
