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
using namespace std;
bool RESTREADOUT_DECODINGFILE_ERROR = false;

ClassImp(TRestReadout);
///////////////////////////////////////////////
/// \brief TRestReadout default constructor
///
TRestReadout::TRestReadout() {
    RESTWarning << "Creating legacy metadata TRestReadout" << RESTendl;
    RESTWarning << "This class is now implemented under TRestDetectorReadout" << RESTendl;
    Initialize();
}

///////////////////////////////////////////////
/// \brief TRestReadout constructor loading data from a config file
///
/// If no configuration path was defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// By default the config file must be specified with full path, absolute or
/// relative.
///
/// First TRestReadout section occurrence will be loaded.
///
/// \param cfgFileName A const char* giving the path to an RML file.
///
TRestReadout::TRestReadout(const char* cfgFileName) : TRestMetadata(cfgFileName) {
    cout << "Loading readout. This might take few seconds" << endl;
    RESTWarning << "Creating legacy metadata TRestReadout" << RESTendl;
    RESTWarning << "This class is now implemented under TRestDetectorReadout" << RESTendl;
    Initialize();

    LoadConfigFromFile(fConfigFileName);
}

///////////////////////////////////////////////
/// \brief TRestReadout constructor loading data from a config file
///
/// If no configuration path was defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// By default the config file must be specified with full path, absolute or
/// relative.
///
/// \param cfgFileName A const char* giving the path to an RML file.
/// \param name The name of the TRestReadout section to be loaded
///
TRestReadout::TRestReadout(const char* cfgFileName, string name) : TRestMetadata(cfgFileName) {
    cout << "Loading readout. This might take few seconds" << endl;
    RESTWarning << "Creating legacy metadata TRestReadout" << RESTendl;
    RESTWarning << "This class is now implemented under TRestDetectorReadout" << RESTendl;
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);
}

///////////////////////////////////////////////
/// \brief Initializes the readout members and defines the section name
///
void TRestReadout::Initialize() {
    SetSectionName(this->ClassName());

    fDecoding = false;

    fNReadoutPlanes = 0;
    fReadoutPlanes.clear();
}

///////////////////////////////////////////////
/// \brief TRestReadout default destructor
///
TRestReadout::~TRestReadout() {}

///////////////////////////////////////////////
/// \brief Returns the number of readout planes defined on the readout
///
Int_t TRestReadout::GetNumberOfReadoutPlanes() { return fReadoutPlanes.size(); }

///////////////////////////////////////////////
/// \brief Returns the **total** number of modules implemented in **all**
/// the readout planes.
///
Int_t TRestReadout::GetNumberOfModules() {
    Int_t modules = 0;
    for (int p = 0; p < GetNumberOfReadoutPlanes(); p++) modules += fReadoutPlanes[p].GetNumberOfModules();
    return modules;
}

///////////////////////////////////////////////
/// \brief Returns the **total** number of channels implemented in **all**
/// the readout planes and modules.
///
Int_t TRestReadout::GetNumberOfChannels() {
    Int_t channels = 0;
    for (int p = 0; p < GetNumberOfReadoutPlanes(); p++)
        for (int m = 0; m < fReadoutPlanes[p].GetNumberOfModules(); m++)
            channels += fReadoutPlanes[p][m].GetNumberOfChannels();
    return channels;
}

///////////////////////////////////////////////
/// \brief Returns a pointer to the readout plane by ID
///
TRestReadoutPlane* TRestReadout::GetReadoutPlaneWithID(int id) {
    for (int i = 0; i < this->GetNumberOfReadoutPlanes(); i++) {
        if (fReadoutPlanes[i].GetID() == id) {
            return &fReadoutPlanes[i];
        }
    }

    return NULL;
}

///////////////////////////////////////////////
/// \brief Returns a pointer to the readout module by ID
///
/// e.g. micromegas M0 has id 0, M5 has id 5. The **ID** is Unique of all the
/// readout mudules
TRestReadoutModule* TRestReadout::GetReadoutModuleWithID(int id) {
    for (int i = 0; i < this->GetNumberOfReadoutPlanes(); i++) {
        TRestReadoutPlane& plane = fReadoutPlanes[i];

        for (int j = 0; j < plane.GetNumberOfModules(); j++) {
            if (plane[j].GetModuleID() == id) {
                return &plane[j];
            }
        }
    }
    return NULL;
}

TRestReadoutChannel* TRestReadout::GetReadoutChannelWithdaqID(int daqId) {
    int planeID = -1, moduleID = -1, channelID = -1;

    GetPlaneModuleChannel(daqId, planeID, moduleID, channelID);
    if (channelID >= 0) {
        return &fReadoutPlanes[planeID][moduleID][channelID];
    }

    return NULL;
}

///////////////////////////////////////////////
/// \brief Returns a pointer to the readout plane by index
///
TRestReadoutPlane* TRestReadout::GetReadoutPlane(int p) {
    if (p < fNReadoutPlanes)
        return &fReadoutPlanes[p];
    else {
        RESTWarning << "TRestReadout::GetReadoutPlane." << RESTendl;
        RESTWarning << "Readout plane index exceeded." << RESTendl;
        RESTWarning << "Index requested : " << p << RESTendl;
        RESTWarning << "Number of readout planes defined : " << fNReadoutPlanes << RESTendl;
    }

    return NULL;
}

///////////////////////////////////////////////
/// \brief Adds a readout plane to the readout
///
void TRestReadout::AddReadoutPlane(TRestReadoutPlane plane) {
    fReadoutPlanes.push_back(plane);
    fNReadoutPlanes++;
}

///////////////////////////////////////////////
/// \brief Initializes the readout members using the information given in
/// the TRestReadout RML section.
///
void TRestReadout::InitFromConfigFile() {
    fMappingNodes = StringToInteger(GetParameter("mappingNodes", "0"));

#pragma region ParseModuledefinition
    vector<TRestReadoutModule> fModuleDefinitions;
    TiXmlElement* moduleDefinition = GetElement("readoutModule");
    while (moduleDefinition != NULL) {
        if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
            cout << "------module-----------------" << endl;
            cout << moduleDefinition << endl;
            cout << "-----------------------------" << endl;
            GetChar();
        }

        TRestReadoutModule module = *ParseModuleDefinition(moduleDefinition);
        // module.DoReadoutMapping(fMappingNodes);
        fModuleDefinitions.push_back(module);
        moduleDefinition = GetNextElement(moduleDefinition);
    }
#pragma endregion
    TiXmlElement* planeDefinition = GetElement("readoutPlane");
    vector<TRestReadoutModule> moduleVector;
    Int_t addedChannels = 0;
    while (planeDefinition != NULL) {
        TRestReadoutPlane plane;

        plane.SetID(GetNumberOfReadoutPlanes());
        plane.SetPosition(Get3DVectorParameterWithUnits("position", planeDefinition));
        plane.SetCathodePosition(Get3DVectorParameterWithUnits("cathodePosition", planeDefinition));
        plane.SetPlaneVector(StringTo3DVector(GetFieldValue("planeVector", planeDefinition)));
        plane.SetChargeCollection(StringToDouble(GetFieldValue("chargeCollection", planeDefinition)));

        Double_t tDriftDistance = plane.GetDistanceTo(plane.GetCathodePosition());
        plane.SetTotalDriftDistance(tDriftDistance);

#pragma region addReadoutModuleToPlane

        moduleVector.clear();
        TiXmlElement* moduleDefinition = GetElement("addReadoutModule", planeDefinition);
        while (moduleDefinition != NULL) {
            TString modName = GetFieldValue("name", moduleDefinition);
            Int_t mid = -1;
            for (unsigned int i = 0; i < fModuleDefinitions.size(); i++) {
                if (fModuleDefinitions[i].GetName() == modName) {
                    mid = i;
                }
            }

            if (mid == -1) {
                RESTError << "TRestReadout at <addReadoutModule>. Module name " << modName << " not found!"
                          << RESTendl;
                RESTError << "Please, check spelling" << RESTendl;
                exit(1);
            }

            fModuleDefinitions[mid].SetModuleID(StringToInteger(GetFieldValue("id", moduleDefinition)));
            if (fModuleDefinitions[mid].GetModuleID() < -1) {
                RESTWarning << "Module id must be positive!" << RESTendl;
                fModuleDefinitions[mid].SetModuleID(-1);  // set to -1 to generate from hash later on
            }
            fModuleDefinitions[mid].SetUniqueName(GetFieldValue("uniqueName", moduleDefinition));
            fModuleDefinitions[mid].SetOrigin(StringTo2DVector(GetFieldValue("origin", moduleDefinition)));
            fModuleDefinitions[mid].SetRotation(StringToDouble(GetFieldValue("rotation", moduleDefinition)));

#pragma region SetupDecodingFile

            Int_t firstDaqChannel = StringToInteger(GetFieldValue("firstDaqChannel", moduleDefinition));
            if (firstDaqChannel == -1) firstDaqChannel = addedChannels;

            string decodingFile = GetFieldValue("decodingFile", moduleDefinition);
            if (decodingFile == PARAMETER_NOT_FOUND_STR || decodingFile == "" ||
                RESTREADOUT_DECODINGFILE_ERROR)
                fDecoding = false;
            else
                fDecoding = true;

            string _decodingFile = SearchFile(decodingFile);

            if (fDecoding && !TRestTools::fileExists(_decodingFile.c_str())) {
                RESTWarning << "The decoding file does not exist!" << RESTendl;
                RESTWarning << "--------------------------------" << RESTendl;
                RESTWarning << "File : " << decodingFile << RESTendl;
                RESTWarning << "Default decoding will be used. readoutChannel=daqChannel" << RESTendl;
                RESTWarning << "To avoid this message and use the default decoding define : "
                               "decodingFile=\"\""
                            << RESTendl;
                RESTWarning << "--------------------------------" << RESTendl;
                RESTWarning << "Press a KEY to continue..." << RESTendl;
                getchar();
                fDecoding = false;
                RESTREADOUT_DECODINGFILE_ERROR = true;
            }

            decodingFile = _decodingFile;

            vector<Int_t> rChannel;
            vector<Int_t> dChannel;
            if (fDecoding && TRestTools::fileExists(decodingFile.c_str())) {
                FILE* f = fopen(decodingFile.c_str(), "r");

                Int_t daq, readout;
                while (!feof(f)) {
                    if (fscanf(f, "%d\t%d\n", &daq, &readout) <= 0) {
                        RESTError << "TRestReadout::InitFromConfigFile. Problem reading decoding" << RESTendl;
                        RESTError << "This error might need support at REST forum" << RESTendl;
                        exit(-1);
                    }
                    // we skip blank daq channels if readout id is <0
                    // e.g. daq id: 22, readout id: -1
                    if (readout >= 0) {
                        rChannel.push_back(readout);
                        dChannel.push_back(daq + firstDaqChannel);
                    }
                }
                fclose(f);
            }

            if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
                cout << "------module-----------------" << endl;
                cout << moduleDefinition << endl;
                cout << "-----------------------------" << endl;
                getchar();
            }

            if (fDecoding && (unsigned int)fModuleDefinitions[mid].GetNumberOfChannels() != rChannel.size()) {
                RESTError << "TRestReadout."
                          << " The number of channels defined in the readout is not the same"
                          << " as the number of channels found in the decoding." << RESTendl;
                exit(1);
            }

            for (int ch = 0; ch < fModuleDefinitions[mid].GetNumberOfChannels(); ch++) {
                if (!fDecoding) {
                    Int_t id = ch;
                    rChannel.push_back(id);
                    dChannel.push_back(id + firstDaqChannel);
                }

                // WRONG version before -->
                // fModuleDefinitions[mid].GetChannel(ch)->SetID( rChannel[ch] );
                fModuleDefinitions[mid].GetChannel(rChannel[ch])->SetDaqID(dChannel[ch]);
                fModuleDefinitions[mid].GetChannel(rChannel[ch])->SetChannelID(rChannel[ch]);

#pragma endregion
                addedChannels++;
            }
            fModuleDefinitions[mid].SetMinMaxDaqIDs();

            moduleVector.push_back(fModuleDefinitions[mid]);
            // plane.AddModule( fModuleDefinitions[mid] );

            moduleDefinition = GetNextElement(moduleDefinition);
        }

        // We removed the constraint that module id's should start by 0 and have no
        // missing numbers in a multi-module readout plane. Modules can have their
        // special "id", e.g. M0, M2, M3, M4 in SJTU proto. We don't have M1
        // edit: 2022/04: modules now have their special names. If so, the id will automatically
        // be assigned if not right.

        for (Int_t i(0); i < (Int_t)moduleVector.size(); i++) {
            if (moduleVector[i].GetModuleID() == -1) {
                if (moduleVector[i].GetUniqueName() != "" &&
                    moduleVector[i].GetUniqueName() != PARAMETER_NOT_FOUND_STR) {
                    moduleVector[i].SetModuleID(abs((int)ToHash(moduleVector[i].GetUniqueName())));
                } else {
                    RESTError
                        << "TRestReadout::InitFromConfigFile() Module id and unique name are both empty!"
                        << RESTendl;
                    exit(0);
                }
            }

            plane.AddModule(moduleVector[i]);
        }
#pragma endregion

        this->AddReadoutPlane(plane);
        planeDefinition = GetNextElement(planeDefinition);
    }

    ValidateReadout();
}

TRestReadoutModule* TRestReadout::ParseModuleDefinition(TiXmlElement* moduleDefinition) {
    TRestReadoutModule* mod = new TRestReadoutModule();
    TRestReadoutModule& module = *mod;
    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Warning) module.EnableWarnings();

    module.SetName(GetFieldValue("name", moduleDefinition));
    module.SetSize(StringTo2DVector(GetFieldValue("size", moduleDefinition)));
    module.SetTolerance(StringToDouble(GetFieldValue("tolerance", moduleDefinition)));

#pragma region addChannel
    vector<TRestReadoutChannel> channelVector;
    vector<int> channelIDVector;
    TiXmlElement* channelDefinition = GetElement("readoutChannel", moduleDefinition);
    while (channelDefinition != NULL) {
        TRestReadoutChannel channel;

        Int_t id = StringToInteger(GetFieldValue("id", channelDefinition));
        if (id != -1) channelIDVector.push_back(id);
        channel.SetDaqID(-1);

#pragma region addPixel
        vector<TRestReadoutPixel> pixelVector;
        vector<int> pixelIDVector;
        TiXmlElement* pixelDefinition = GetElement("addPixel", channelDefinition);
        while (pixelDefinition != NULL) {
            TRestReadoutPixel pixel;

            pixel.SetOrigin(StringTo2DVector(GetFieldValue("origin", pixelDefinition)));
            pixel.SetSize(StringTo2DVector(GetFieldValue("size", pixelDefinition)));
            pixel.SetRotation(StringToDouble(GetFieldValue("rotation", pixelDefinition)));
            pixel.SetTriangle(StringToBool(GetFieldValue("triangle", pixelDefinition)));

            if (StringToInteger(GetFieldValue("id", pixelDefinition)) != -1)
                pixelIDVector.push_back(StringToInteger(GetFieldValue("id", pixelDefinition)));
            pixelVector.push_back(pixel);
            // channel.AddPixel( pixel );
            pixelDefinition = GetNextElement(pixelDefinition);
        }

        if (pixelIDVector.size() > 0 && pixelIDVector.size() != pixelVector.size()) {
            RESTError
                << "pixel id definition may be wrong! It must be coherent and starts from 0. Check your "
                   "readout module definition!"
                << RESTendl;
            exit(0);
        }

        // Creating the vector fReadoutPixel in the channel with pixels added in the
        // order of their ID.
        for (Int_t i(0); i < (Int_t)pixelVector.size(); i++) {
            for (Int_t j(0); j < (Int_t)pixelVector.size(); j++) {
                if (pixelIDVector[j] == i) {
                    channel.AddPixel(pixelVector[j]);
                    break;
                }
            }
        }

        if (channel.GetNumberOfPixels() != pixelVector.size()) {
            RESTError << "pixel id definition may be wrong! check your "
                         "readout module definition!"
                      << RESTendl;
            exit(0);
        }
#pragma endregion

        channelVector.push_back(channel);
        channelDefinition = GetNextElement(channelDefinition);
    }

    if (channelIDVector.size() > 0 && channelIDVector.size() != channelVector.size()) {
        RESTError << "TRestReadout::ParseModuleDefinition. Channel id definition may be wrong!"
                  << "check your readout module definition!" << RESTendl;
        RESTError << " " << RESTendl;
        RESTError << "channelIDVector size : " << channelIDVector.size() << RESTendl;
        RESTError << "channel vector size : " << channelVector.size() << RESTendl;

        exit(0);
    }

    // Creating the vector fReadoutChannel in the module with channels added in
    // the order of their ID.
    for (Int_t i(0); i < (Int_t)channelVector.size(); i++) {
        for (Int_t j(0); j < (Int_t)channelVector.size(); j++) {
            if (channelIDVector[j] == i) {
                module.AddChannel(channelVector[j]);
                break;
            }
        }
    }

    if (module.GetNumberOfChannels() != channelVector.size()) {
        RESTError << "TRestReadout::ParseModuleDefinition. Channel id definition may be wrong!"
                  << "check your readout module definition!" << RESTendl;
        RESTError << " " << RESTendl;
        RESTError << "Module number of channels : " << module.GetNumberOfChannels() << RESTendl;
        RESTError << "channel vector size : " << channelVector.size() << RESTendl;

        exit(0);
    }
#pragma endregion

    return mod;
}

///////////////////////////////////////////////
/// \brief Starts the readout mapping initialization. This process
/// is computationally expensive but it greatly optimizes the FindChannel
/// process later on.
///
void TRestReadout::DoReadoutMapping(TRestReadoutModule* mod, int nodes) {
    ///////////////////////////////////////////////////////////////////////////////
    // We initialize the mapping readout net to sqrt(numberOfPixels)
    // However this might not be good for readouts where the pixels are
    // assymmetric
    // /////////////////////////////////////////////////////////////////////////////
    Int_t totalNumberOfPixels = 0;
    for (int ch = 0; ch < mod->GetNumberOfChannels(); ch++)
        totalNumberOfPixels += mod->GetChannel(ch)->GetNumberOfPixels();

    if (nodes == 0) {
        nodes = TMath::Sqrt(totalNumberOfPixels);
        nodes = 2 * nodes;
    }

    cout << "Performing readout mapping optimization (This might require long "
            "computation time)"
         << endl;
    cout << "--------------------------------------------------------------------"
            "--------------"
         << endl;
    cout << "Total number of pixels : " << totalNumberOfPixels << endl;
    cout << "Nodes : " << nodes << endl;

    TRestReadoutMapping& fMapping = fReadoutMappings[mod->GetName()];
    fMapping.Initialize(nodes, nodes, mod->GetModuleSizeX(), mod->GetModuleSizeY());

    // 1. loop for each pixel, find the mapping nodes for the pixel position,
    // and assign the pixel/channel id to the nodes.
    for (int ch = 0; ch < mod->GetNumberOfChannels(); ch++) {
        for (int px = 0; px < mod->GetChannel(ch)->GetNumberOfPixels(); px++) {
            Double_t xPix = mod->GetChannel(ch)->GetPixel(px)->GetCenter().X();
            Double_t yPix = mod->GetChannel(ch)->GetPixel(px)->GetCenter().Y();

            Int_t nodeX = fMapping.GetNodeX(xPix);
            Int_t nodeY = fMapping.GetNodeY(yPix);

            // This means that two pixels in the readout are associated to the same
            // node.
            // If the granularity of the readout is not high enough this may happen
            // often. This should be just a RESTWarning I guess.
            if (fMapping.isNodeSet(nodeX, nodeY)) {
                cout << endl;
                RESTWarning << "TRestReadoutModule. Node is already SET!!" << RESTendl;
                RESTWarning << "Trying to associate channel : " << ch << " Pixel : " << px << RESTendl;
                RESTWarning << "Pixel coordinates : ( " << xPix << " , " << yPix << " ) " << RESTendl;

                Int_t tempCh = fMapping.GetChannelByNode(nodeX, nodeY);
                Int_t tempPix = fMapping.GetPixelByNode(nodeX, nodeY);
                RESTWarning << "Already associated channel : " << tempCh << " pixel : " << tempPix
                            << RESTendl;
                Double_t xP = mod->GetChannel(tempCh)->GetPixel(tempPix)->GetCenter().X();
                Double_t yP = mod->GetChannel(tempCh)->GetPixel(tempPix)->GetCenter().Y();
                RESTWarning << "Pixel coordinates : ( " << xP << " , " << yP << " ) " << RESTendl;
                cout << endl;

                cout << "Increasing the number of mapping of nodes may solve this issue." << endl;
                cout << endl;
            }
            fMapping.SetNode(nodeX, nodeY, ch, px);
        }
    }

    // 2. There should still be some nodes without channel/pixel id assignment. Now
    // we loop for all the nodes and find the pixel on their position
    for (int i = 0; i < nodes; i++) {
        printf("Completed : %.2lf %%\r", 100. * (i * (Double_t)nodes) / nodes / nodes);
        fflush(stdout);
        for (int j = 0; j < nodes; j++) {
            Double_t x = fMapping.GetX(i);
            Double_t y = fMapping.GetY(j);

            if (!fMapping.isNodeSet(i, j)) {
                // find the nearby nodes with channel/pixel ids set
                // const int NNearby = 5;
                // for (int ii = i - NNearby; ii <= i + NNearby; ii++) {
                //    if (ii < 0 || ii >= nodes || ii == i) continue;
                //    for (int jj = j - NNearby; jj <= j + NNearby; jj++) {
                //        if (jj < 0 || jj >= nodes || jj == i) continue;

                //        if (fMapping.isNodeSet(ii, jj)) {
                //            Int_t ch = fMapping.GetChannelByNode(ii, jj);
                //            Int_t px = fMapping.GetPixelByNode(ii, jj);

                //            if (mod->isInsidePixelRelative(ch, px, x, y)) {
                //                fMapping.SetNode(i, j, ch, px);
                //            }
                //        }
                //    }
                //}

                // if cannot find pixel within nearby 10*10 nodes, do a thorough scan
                if (!fMapping.isNodeSet(i, j)) {
                    for (int ch = 0; ch < GetNumberOfChannels() && !fMapping.isNodeSet(i, j); ch++) {
                        for (int px = 0;
                             px < mod->GetChannel(ch)->GetNumberOfPixels() && !fMapping.isNodeSet(i, j);
                             px++) {
                            if (mod->isInsidePixelRelative(ch, px, x, y)) {
                                fMapping.SetNode(i, j, ch, px);
                            }
                        }
                    }
                }
            }
        }
    }

    if (!fMapping.AllNodesSet())
        cout << "Not all nodes set" << endl;
    else
        cout << "All Nodes set" << endl;

    // save this generated mapping
    for (int i = 0; i < fReadoutPlanes.size(); i++) {
        TRestReadoutPlane* p = &fReadoutPlanes[i];
        for (int j = 0; j < p->GetNumberOfModules(); j++) {
            TRestReadoutModule* m = p->GetModule(j);
            if (m->GetName() == mod->GetName()) {
                m->SetReadoutMapping(&fMapping);
            }
        }
    }
    return;
}

///////////////////////////////////////////////
/// \brief This method is not implemented yet. But it could
/// do some checks to help verifying the readout.
///
void TRestReadout::ValidateReadout() {
    // cout << "--------------------------------------------------" << endl;
    // cout << "TRestReadout::ValidateReadout:: NOT IMPLEMENTED" << endl;
    // cout << "This function should crosscheck that there are no repeated "
    //        "DaqChannels IDs"
    //     << endl;
    // cout << "If any checks are implemented in the future. Those checks should be "
    //        "an option."
    //     << endl;
    // cout << "No dead area in the readout module" << endl;
    // cout << "And other checks" << endl;
    // cout << "--------------------------------------------------" << endl;
}

void TRestReadout::GetPlaneModuleChannel(Int_t signalID, Int_t& planeID, Int_t& moduleID, Int_t& channelID) {
    for (int p = 0; p < GetNumberOfReadoutPlanes(); p++) {
        TRestReadoutPlane* plane = &fReadoutPlanes[p];
        for (int m = 0; m < plane->GetNumberOfModules(); m++) {
            TRestReadoutModule* mod = &(*plane)[m];

            if (mod->isDaqIDInside(signalID)) {
                planeID = plane->GetID();
                moduleID = mod->GetModuleID();
                channelID = mod->DaqToReadoutChannel(signalID);
            }
        }
    }
}

Int_t TRestReadout::GetHitsDaqChannel(TVector3 hitpos, Int_t& planeID, Int_t& moduleID, Int_t& channelID) {
    double x = hitpos.X();
    double y = hitpos.Y();
    double z = hitpos.Z();

    for (int p = 0; p < GetNumberOfReadoutPlanes(); p++) {
        TRestReadoutPlane* plane = &fReadoutPlanes[p];
        int m = plane->GetModuleIDFromPosition(x, y, z);
        if (m >= 0) {
            // TRestReadoutModule* mod = plane->GetModuleByID(m);
            TRestReadoutModule* mod = plane->GetModuleByID(m);

            if (mod->GetMapping() == NULL) {
                if (fReadoutMappings.count(mod->GetName()) != 0) {
                    mod->SetReadoutMapping(&fReadoutMappings[mod->GetName()]);
                } else {
                    DoReadoutMapping(mod, fMappingNodes);
                }
            }

            Int_t readoutChannel = mod->FindChannel(x, y);
            if (readoutChannel >= 0) {
                planeID = plane->GetID();
                moduleID = mod->GetModuleID();
                channelID = readoutChannel;
                return mod->GetChannel(readoutChannel)->GetDaqID();
            }
        }
    }
    return -1;
}

Double_t TRestReadout::GetX(Int_t signalID) {
    Int_t planeID, readoutChannel = -1, readoutModule;
    GetPlaneModuleChannel(signalID, planeID, readoutModule, readoutChannel);
    if (readoutChannel == -1) {
        // cout << "REST Warning : Readout channel not found for daq ID : " << signalID << endl;
        return numeric_limits<Double_t>::quiet_NaN();
    }
    return GetX(planeID, readoutModule, readoutChannel);
}

Double_t TRestReadout::GetY(Int_t signalID) {
    Int_t planeID, readoutChannel = -1, readoutModule;
    GetPlaneModuleChannel(signalID, planeID, readoutModule, readoutChannel);
    if (readoutChannel == -1) {
        // cout << "REST Warning : Readout channel not found for daq ID : " << signalID << endl;
        return numeric_limits<Double_t>::quiet_NaN();
    }
    return GetY(planeID, readoutModule, readoutChannel);
}
///////////////////////////////////////////////
/// \brief It returns the x-coordinate for the given readout
/// plane, *plane*, a given module, *modID*, and a given
/// channel, *chID*.
///
Double_t TRestReadout::GetX(Int_t plane, Int_t modID, Int_t chID) {
    return GetReadoutPlaneWithID(plane)->GetX(modID, chID);
}

///////////////////////////////////////////////
/// \brief It returns the y-coordinate for the given readout
/// plane, *plane*, a given module, *modID*, and a given
/// channel, *chID*.
///
Double_t TRestReadout::GetY(Int_t plane, Int_t modID, Int_t chID) {
    return GetReadoutPlaneWithID(plane)->GetY(modID, chID);
}

///////////////////////////////////////////////
/// \brief Prints on screen the details about the readout
/// definition.
///
/// \param fullDetail Prints all modules, channels and pixels info.
///
void TRestReadout::PrintMetadata(Int_t DetailLevel) {
    if (DetailLevel >= 0) {
        TRestMetadata::PrintMetadata();

        RESTMetadata << "Number of readout planes : " << fNReadoutPlanes << RESTendl;
        RESTMetadata << "Decoding was defined : ";
        if (fDecoding)
            RESTMetadata << "YES" << RESTendl;
        else
            RESTMetadata << "NO" << RESTendl;
        RESTMetadata << "-----------------------------------" << RESTendl;
        for (int p = 0; p < GetNumberOfReadoutPlanes(); p++) fReadoutPlanes[p].Print(DetailLevel - 1);
        RESTMetadata << "****************************************" << RESTendl;
        cout << RESTendl;
    }
}

///////////////////////////////////////////////
/// \brief Draws the readout on screen. Not yet implemented.
///
void TRestReadout::Draw() {
    cout << " TRestReadout::Draw() is not implemented" << endl;
    cout << " To draw a TRestReadout class with name \"readoutName\"";
    cout << " stored in a ROOT file \"rootFile.root\"" << endl;
    cout << " You can use the script : REST_Readout_Viewer( \"rootFile.root\", "
            "\"readoutName\" )"
         << endl;
    cout << endl;
    cout << " Or you can access directly a readout plane and draw using : " << endl;
    cout << " readout->GetReadoutPlane( 0 )->Draw( ); " << endl;
}
