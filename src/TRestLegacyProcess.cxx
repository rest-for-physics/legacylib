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
/// The TRestLegacyProcess is the base class for legacy processes, which
/// stands for processes which are not part of REST anymore but they are
/// kept to keep backward compatibility with previous REST realeases.
/// The creation of a legacy process is not allowed, you will have some
/// errors and warnings in case you attempt to run a legacy process.
/// RESTsoft - Software for Rare Event Searches with TPCs
///
///----------------------------------------------------------------------
///
/// REST-for-Physics - Software for Rare Event Searches Toolkit
///
/// History of developments:
///
/// 2022-05: First implementation of TRestLegacyProcess
/// JuanAn Garcia
///
/// \class TRestLegacyProcess
/// \author: JuanAn Garcia. Write full name and e-mail: juanangp@unizar.es
///
/// <hr>
///

#include "TRestLegacyProcess.h"

ClassImp(TRestLegacyProcess);
