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
/// The TRestSignalZeroSuppresionProcess identifies the points that are over
/// threshold from the input TRestRawSignalEvent. The resulting points, that
/// are presumed to be a physical signal, will be transported to the output
/// TRestSignalEvent returned by this process. The data points transferred to
/// the output TRestSignalEvent will have physical time units related to the
/// sampling rate of the raw signal received as input, and defined as a
/// parameter in this process.
///
/// The different parameters required by this process are:
/// * baselineRange : A 2D-vector definning the range, in number of bins,
/// where the baseline properties will be calculated.
/// * integralRange : A 2D-vector definning the time window, in number of bins,
/// where the signal will be considered.
/// * pointThreshold : The number of sigmas over the baseline flunctuations to
/// consider a point is over the threshold.
/// * pointsOverThreshold : The number of consecutive points over threshold
/// required to consider them as a physical signal.
/// * signalThreshold : The number of sigmas a set of consecutive points
/// identified over threshold must be over the baseline fluctuations to be
/// finally considered a physical signal.
/// * pointsFlatThreshold : A parameter to help removing the un-physical or
/// un-expected tail that follows the physical signal. \TODO more details?
/// * sampling : The time duration of a time bin from the input TRestRawSignalEvent.
/// If no units are specified, the default units, microseconds, will be
/// considered.
///
/// \TODO Add description of observables here.
///
///
/// An example of definition of this process inside a data processing chain,
/// inside the `<TRestProcessRunner>` section.
///
/// The values given between `${}` are enviroment variables that can be defined
/// at the system or at the `<globals>` section. See also TRestMetadata for
/// additional details.
///
/// \code
///   <addProcess type="TRestSignalZeroSuppresionProcess" name="zS" value="ON"
///               baseLineRange="(${BL_MIN},${BL_MAX})"
///               integralRange="(${INT_MIN},${INT_MAX})"
///               pointThreshold="${POINT_TH}"
///               pointsOverThreshold="${NPOINTS}"
///               sampling="${SAMPLING}"
///               signalThreshold="${SGNL_TH}"
///               observable="all"
///               verboseLevel="silent"   />
/// \endcode
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-January: Conception and implementation of signal zero suppression
/// process.
///               Javier Galan
///
/// \class      TRestSignalZeroSuppresionProcess
/// \author     Javier Galan
/// \author     Kaixiang Ni
///
/// <hr>
///

#include "TRestSignalZeroSuppresionProcess.h"