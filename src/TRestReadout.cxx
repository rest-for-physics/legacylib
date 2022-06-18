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
///
/// This is the main RESTMetadata readout class of REST. The readout class
/// defines the methods allowing to stablish a relation between the hits
/// inside the TPC and the signals read out by the electronics daq.
///
/// A full readout description is composed by at least one readout plane,
/// (TRestReadoutPlane), where we can place any number of readout modules
/// (TRestReadoutModule). A readout module is composed by readout channels
/// (TRestReadoutChannel) which describe the basic active detection area,
/// which can take any complex shape by combinning primitive geometry
/// elements (TRestReadoutPixel).
///
/// REST processes such as TRestSignalToHitsProcess and
/// TRestHitsToSignalProcess use the TRestReadout class to transform the
/// spatial coordinates into raw signal data, and viceversa.
///
/// ### Constructing the readout through an RML file
///
/// The readout definition must be initialized through a RML file. The
/// basic RESTMetadata structure of a readout follows this scheme
///
/// \code
/// <section TRestReadout ... >
///
///     <parameter name="mappingNodes" value="100" />
///
///     // First, we need to construct the different readout modules we will
///     // use inside the readout plane definition.
///     <readoutModule name="modName" size="(100,100)" tolerance="1.e-4" >
///
///         // Later, when we construct the readout plane we will use "modName"
///         // to add the module to the readout plane definition.
///
///         <readoutChannel id="0">
///             <addPixel id="0" origin="(0,0)" size="(10,10)" rotation="45" />
///             <addPixel id="1" origin="(10,10)" size="(10,10)" rotation="45"
///             />
///             ...
///             // Add as many pixel definitions as needed to create the channel
///             definition
///             ...
///         </readoutChannel>
///
///         // And add as many readout channels as need to construct your
///         readout module <readoutChannel id="1">
///             ...
///             ...
///         </readoutChannel>
///
///         // The id number given to the readout channel is the physical
///         readout
///         // channel used inside the decoding.
///         <readoutChannel id="n">
///             ...
///             ...
///         </readoutChannel>
///
///     </readoutModule>
///
///     // If your readout is composed by modules with different size or channel
///     topologies,
///     // you will need add additional <readoutModule ...> definitions here.
///
///     // The readout plane allows to define the position, the orientation
///     (planeVector),
///     // and the position of the cathode that will define the detector active
///     volume. <readoutPlane position="(0,0,-990)" units="mm"
///                   planeVector="(0,0,1)"  chargeCollection="1"
///                   cathodePosition="(0,0,0)" units="mm" >
///
///            // We can then insert the modules inside each readout plane
///            <addReadoutModule name="modName" // We use the previously defined
///            readout module
///                              origin="(0,0)" // Position of the left-bottom
///                              corner (rotation point). rotation="0"   //
///                              Rotation in degrees around position.
///                              decodingFile="module.dec"  // A file describing
///                              the decoding firstDaqChannel="0" >  // Offset
///                              introduced to the daq signal channel inside the
///                              decoding file.
///
///            <addReadoutModule name="modName"
///                              origin="(0,0)" rotation="0"
///                              decodingFile="module.dec" firstDaqChannel="272"
///                              >
///
///           ...
///
///           // Add as many modules as needed to construct your readout.
///     </readoutPlane>
///
///     // If needed you may add any additional readout layers to your
///     definition adding new
///     // readout plane <readoutPlane ...> definitions here.
///
///
/// </section>
/// \endcode
///
///
/// The *mappingNodes* parameter allows to specify the size of the virtual grid
/// that will be used to map the readout. The mapping allows to speed up the
/// process finding a pixel inside a module for a given x,y coordinates.
/// In general, the number of mapping nodes should be high enough so that every
/// pixel from any readout channel is associated to, or contains, a node in the
/// grid. However, as higher is the number of nodes in the mapping grid, higher
/// will be the required computation time to find a pixel for a given x,y
/// coordinates. If this value is not defined REST will try to find an optimum
/// value for this parameter. Therefore, it is recommended to do not specify
/// this parameter, except for solving readout problems or optimization
/// purposes.
///
/// ### The decoding
///
/// The relation between the channel number imposed by the electronic
/// acquisition and the readout channel id defined inside *readoutChannel* is
/// given to TRestReadout through a decoding file.
///
/// The *decodingFile* parameter inside each module added to the readout plane
/// allows to set the filename (in the code above, *module.dec*) of a file
/// used to define the relation between the physical readout *id* defined
/// at each readout channel, as <code> <readoutChannel id="n"> </code>
/// and the daq channel number defined at the acquisition system.
/// If *no decoding* file is defined the relation between daq and readout
/// channel is assigned *one to one*.
/// The decoding file must be a text file definning two columns with as
/// many columns as the number of channels defined in the readout module.
/// The first column is the daq channel number, and the second column is
/// the readout channel defined in the RML file.
///
/// An example of contents of a decoding file. Here 0, 1, 2, 3, ... are
/// the physical readout channels corresponding to the daq channels 67,
/// 65, 63, 61, ...
///
/// \code
/// 67      0
/// 65      1
/// 63      2
/// 61      3
/// 59      4
/// 57      5
/// 54      6
/// ...
/// ...
/// \endcode
///
/// The channel number defined on the left column will be shifted by the
/// value *firstDaqChannel* defined when adding the readout module to the
/// readout plane. This may allow to re-use a decoding file for different
/// readout modules in case we have a repetitive connection pattern.
///
/// ### Using the readout class
///
/// Once TRestReadout has been initialized, through and RML file or a
/// previously stored TRestReadout class stored in a ROOT file,
/// we can find the corresponding xy-position for a given readout channel
/// id, module id, and readout plane, or the corresponding coordinates for
/// a given channel inside a module containned in a readout plane.
///
/// The following code shows how we can identify the readout plane
/// (*planeId*), the readout module (*moduleId*), and the readout channel
/// (*chanId*) for a given x,y,z coordinates.
///
/// \code
///        for( int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++ )
///        {
///            // We check if the xyz coordinates are in the volume defined by
///            the readout plane modId =
///            fReadout->GetReadoutPlane(p)->isInsideDriftVolume( x, y, z );
///
///            // If the hit is containned in the readout, the last function
///            returns the id of the
///            // readout module that contains the x,y coordinates.
///            if( modId >= 0 )
///            {
///                // We found the hit at planeId
///                planeId = p;
///
///                // Then we obtain the readoutChannel *id* as defined in the
///                readout. Int_t chanId = fReadout->GetReadoutPlane( p
///                )->FindChannel( modId, x, y );
///
///                break;
///            }
///        }
/// \endcode
///
/// Once we found the readout channel, we can obtain the associated daq
/// channel number, that was defined using the decoding file.
///
/// \code
///            Int_t daqId = fReadout->GetReadoutPlane( planeId )
///                                  ->GetModule( modId )
///                                  ->GetChannel( chanId )->GetDaqID( );
/// \endcode
///
/// The other way around we can obtain the corresponding x and y coordinates for
/// a given daq channel id. The following code shows the basic idea to find
/// *signalId*.
///
/// \code
///        for( int p = 0; p < fReadout->GetNumberOfReadoutPlanes(); p++ )
///        {
///            TRestReadoutPlane *plane = fReadout->GetReadoutPlane( p );
///            for( int m = 0; m < plane->GetNumberOfModules(); m++ )
///            {
///                TRestReadoutModule *mod = plane->GetModule( m );
///
///                // We iterate over all readout modules searching for the one
///                that contains our signal id if( mod->isDaqIDInside( signalID
///                ) )
///                {
///                    // If we find it we use the readoutModule id, and the
///                    signalId corresponding
///                    // to the physical readout channel and obtain the x,y
///                    coordinates.
///
///                    readoutChannelID = mod->DaqToReadoutChannel( signalID );
///                    readoutModuleID = mod->GetModuleID();
///
///                    x = plane->GetX( readoutModuleID, readoutChannelID );
///                    y = plane->GetY( readoutModuleID, readoutChannelID );
///                    break;
///               }
///            }
///         }
/// \endcode
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2015-aug:  First concept.
///            Javier Galan
///
/// \class      TRestReadout
/// \author     Javier Galan
///
/// <hr>
///
///

#include "TRestReadout.h"