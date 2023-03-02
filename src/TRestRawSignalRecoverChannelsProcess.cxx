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
/// The TRestRawSignalRecoverChannelsProcess allows to recover a selection
/// of daq channel ids from a TRestRawSignalEvent. The dead channels must
/// be known beforehand and the signal ids to be recovered must be
/// specified through the corresponding section at the RML configuration
/// file.
///
/// The following example will apply the recovery algorithm for the
/// channels with signal ids 17,19,27 and 67. The signal ids must exist
/// in the readout defined through the TRestDetectorReadout structure.
///
/// \code
/// <TRestRawSignalRecoverChannelsProcess name="returnChannels"
/// title="Recovering few channels" verboseLevel="debug" >
///     <parameter name="channelIds" value="{17,27,67}" />
/// </TRestRawSignalRecoverChannelsProcess>
/// \endcode
///
/// The dead channel reconstruction algorithm is for the moment very
/// simple. The charge of the dead channel is directly calculated by
/// using the charge of the adjacent readout channels,
/// \f$s_i = 0.5 \times (s_{i-1} + s_{i+1})\f$
///
/// This process will access the information of the decoding stored in
/// the TRestDetectorReadout definition to assure that the righ signal ids,
/// corresponding to the adjacent channels, are used in the calculation.
///
/// \warning This process will only be functional if the detectorlib
/// was compiled. You may check if it is the case executing the command
/// `rest-config --libs`, and checking the output shows `-lRestDetector`.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2017-November: First implementation of TRestRawSignalRecoverChannelsProcess.
///             Javier Galan
///
/// \class      TRestRawSignalRecoverChannelsProcess
/// \author     Javier Galan
///
/// <hr>
///
#include "TRestRawSignalRecoverChannelsProcess.h"

ClassImp(TRestRawSignalRecoverChannelsProcess);
