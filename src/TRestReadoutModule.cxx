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
/// This class stores the readout module geometrical description, module
/// position, orientation, and size. It contains a vector of
/// TRestReadoutChannel with the definition of the readout channels
/// existing in the readout module.
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2015-aug:  First concept.
///            Javier Galan
///
/// \class      TRestReadoutModule
/// \author     Javier Galan
///
/// <hr>
///

#include "TRestReadoutModule.h"
using namespace std;

ClassImp(TRestReadoutModule);
///////////////////////////////////////////////
/// \brief Default TRestReadoutModule constructor
///
TRestReadoutModule::TRestReadoutModule() { Initialize(); }

///////////////////////////////////////////////
/// \brief Default TRestReadoutModule destructor
///
TRestReadoutModule::~TRestReadoutModule() {}

///////////////////////////////////////////////
/// \brief TRestReadoutModule initialization
///
void TRestReadoutModule::Initialize() {
    fReadoutChannel.clear();
    fModuleID = -1;

    fMapping = NULL;

    fModuleOriginX = 0;
    fModuleOriginY = 0;

    fModuleSizeX = 0;
    fModuleSizeY = 0;

    fModuleRotation = 0;

    fMaximumDaqId = -1;
    fMininimumDaqId = -1;

    fTolerance = 1.e-6;

    showWarnings = false;
}

///////////////////////////////////////////////
/// \brief Initializes the max and min values for the daq channel number
///
void TRestReadoutModule::SetMinMaxDaqIDs() {
    Int_t maxID = GetChannel(0)->GetDaqID();
    Int_t minID = GetChannel(0)->GetDaqID();
    for (int ch = 0; ch < this->GetNumberOfChannels(); ch++) {
        Int_t daqID = GetChannel(ch)->GetDaqID();
        if (daqID > maxID) maxID = daqID;

        if (daqID < minID) minID = daqID;
    }

    fMaximumDaqId = maxID;
    fMininimumDaqId = minID;
}

///////////////////////////////////////////////
/// \brief Determines if a given *daqID* number is in the range of the module
///
Bool_t TRestReadoutModule::isDaqIDInside(Int_t daqID) {
    if (daqID >= fMininimumDaqId && daqID <= fMaximumDaqId) return true;
    return false;
}

///////////////////////////////////////////////
/// \brief Returns the channel index corresponding to the absolute coordinates,
/// i.e, relative to the readout plane coordinate system
///
/// The readout mapping (see TRestReadoutMapping) is used to help finding
/// the pixel where coordinates absX and absY fall in.
///
Int_t TRestReadoutModule::FindChannel(Double_t absX, Double_t absY) {
    if (!isInside(absX, absY)) return -1;

    Double_t x = TransformToModuleCoordinates(absX, absY).X();
    Double_t y = TransformToModuleCoordinates(absX, absY).Y();

    Int_t nodeX = fMapping->GetNodeX(x);
    Int_t nodeY = fMapping->GetNodeY(y);

    Int_t channel = fMapping->GetChannelByNode(nodeX, nodeY);
    Int_t pixel = fMapping->GetPixelByNode(nodeX, nodeY);

    Int_t repeat = 1;
    Int_t count = 0;
    Int_t forward = 1;
    Int_t xAxis = 1;

    Int_t totalNodes = fMapping->GetNumberOfNodesX() * fMapping->GetNumberOfNodesY();

    // We test if x,y is inside the channel/pixel obtained from the readout
    // mapping If not we start to look in the readout mapping neighbours
    while (!this->isInsidePixel(channel, pixel, absX, absY)) {
        count++;
        if (xAxis == 1 && forward == 1)
            nodeX++;
        else if (xAxis == 0 && forward == 1)
            nodeY++;
        else if (xAxis == 1 && forward == 0)
            nodeX--;
        else if (xAxis == 0 && forward == 0)
            nodeY--;

        Int_t nNodes = fMapping->GetNumberOfNodesX();

        if (nodeX < 0) nodeX = nNodes - 1;
        if (nodeY < 0) nodeY = nNodes - 1;
        if (nodeX >= nNodes) nodeX = 0;
        if (nodeY >= nNodes) nodeY = 0;

        if (count >= repeat) {
            if (xAxis == 1 && forward == 1) {
                xAxis = 0;
                forward = 0;
            } else if (xAxis == 0 && forward == 0) {
                xAxis = 1;
                forward = 0;
                repeat++;
            } else if (xAxis == 1 && forward == 0) {
                xAxis = 0;
                forward = 1;
            } else if (xAxis == 0 && forward == 1) {
                xAxis = 1;
                forward = 1;
                repeat++;
            }

            count = 0;
        }

        channel = fMapping->GetChannelByNode(nodeX, nodeY);
        pixel = fMapping->GetPixelByNode(nodeX, nodeY);

        if (count > totalNodes / 10) {
            cout << "REST Error? I did not find any channel for hit position (" << x << "," << y << ")"
                 << endl;

            for (int ch = 0; ch < GetNumberOfChannels(); ch++)
                for (int px = 0; px < GetChannel(ch)->GetNumberOfPixels(); px++)
                    if (isInsidePixel(ch, px, absX, absX)) {
                        cout << "( " << x << " , " << y << ") Should be in channel " << ch
                             << " pixel : " << px << endl;

                        cout << "Corresponding node :  nX: " << fMapping->GetNodeX_ForChannelAndPixel(ch, px)
                             << " nY : " << fMapping->GetNodeY_ForChannelAndPixel(ch, px) << endl;
                        cout << "Channel : " << ch << " Pixel : " << px << endl;
                        cout << "Pix X : " << GetChannel(ch)->GetPixel(px)->GetCenter().X()
                             << " Pix Y : " << GetChannel(ch)->GetPixel(px)->GetCenter().Y() << endl;
                    }
            sleep(5);
            //   getchar();
            return -1;
        }
    }

    return channel;
}

///////////////////////////////////////////////
/// \brief Determines if the position *x,y* (in readout plane coordinate) relative to the readout
/// plane are inside this readout module.
///
Bool_t TRestReadoutModule::isInside(Double_t x, Double_t y) {
    TVector2 v(x, y);
    return isInside(v);
}

///////////////////////////////////////////////
/// \brief Determines if the position TVector2 *pos* (in readout plane coordinate) relative to the readout
/// plane are inside this readout module.
///
Bool_t TRestReadoutModule::isInside(TVector2 pos) {
    TVector2 rotPos = TransformToModuleCoordinates(pos);

    if (rotPos.X() >= 0 && rotPos.X() < fModuleSizeX)
        if (rotPos.Y() >= 0 && rotPos.Y() < fModuleSizeY) return true;

    return false;
}

///////////////////////////////////////////////
/// \brief Determines if the position *x,y* (in readout plane coordinate) is found in any of the pixels
/// of the readout *channel* index given.
///
Bool_t TRestReadoutModule::isInsideChannel(Int_t channel, Double_t x, Double_t y) {
    TVector2 pos(x, y);

    return isInsideChannel(channel, pos);
}

///////////////////////////////////////////////
/// \brief Determines if the position TVector2 *pos* (in readout plane coordinate) is found in any of the
/// pixels of the readout *channel* index given.
///
Bool_t TRestReadoutModule::isInsideChannel(Int_t channel, TVector2 pos) {
    pos = TransformToModuleCoordinates(pos);
    for (int idx = 0; idx < GetChannel(channel)->GetNumberOfPixels(); idx++)
        if (GetChannel(channel)->GetPixel(idx)->isInside(pos)) return true;
    return false;
}

///////////////////////////////////////////////
/// \brief Determines if the position *x,y* (in readout plane coordinate) is found at a specific *pixel* id
/// inside the readout *channel* given.
///
Bool_t TRestReadoutModule::isInsidePixel(Int_t channel, Int_t pixel, Double_t x, Double_t y) {
    TVector2 pos(x, y);

    if (channel < 0 || pixel < 0) return false;

    return isInsidePixel(channel, pixel, pos);
}

///////////////////////////////////////////////
/// \brief Determines if the position TVector2 *pos* (in readout plane coordinate) is found at a specific
/// *pixel* id inside the readout *channel* given.
///
Bool_t TRestReadoutModule::isInsidePixel(Int_t channel, Int_t pixel, TVector2 pos) {
    if (channel < 0 || pixel < 0) return false;

    pos = TransformToModuleCoordinates(pos);
    if (GetChannel(channel)->GetPixel(pixel)->isInside(pos)) return true;
    return false;
}

///////////////////////////////////////////////
/// \brief Determines if the position *x,y* (relative to module coordinate) is found at a specific *pixel* id
/// inside the readout *channel* given.
///
Bool_t TRestReadoutModule::isInsidePixelRelative(Int_t channel, Int_t pixel, Double_t x, Double_t y) {
    TVector2 pos(x, y);

    if (channel < 0 || pixel < 0) return false;

    return isInsidePixelRelative(channel, pixel, pos);
}

///////////////////////////////////////////////
/// \brief Determines if the position TVector2 *pos* (relative to module coordinate) is found at a specific
/// *pixel* id inside the readout *channel* given.
///
Bool_t TRestReadoutModule::isInsidePixelRelative(Int_t channel, Int_t pixel, TVector2 pos) {
    if (channel < 0 || pixel < 0) return false;

    if (GetChannel(channel)->GetPixel(pixel)->isInside(pos)) return true;
    return false;
}

///////////////////////////////////////////////
/// \brief Creates a TVector2 with shortest norm going from the given position
/// *pos* to the module.
///  It can be seen as the vector to add to move from the position to the
///  closest border of the module.
///
TVector2 TRestReadoutModule::GetDistanceToModule(TVector2 pos) {
    TVector2 newPos = TransformToModuleCoordinates(pos);

    Double_t dx = 0, dy = 0;
    if (newPos.X() < 0)
        dx = -newPos.X();
    else if (fModuleSizeX - newPos.X() < 0)
        dx = fModuleSizeX - newPos.X();

    if (newPos.Y() < 0)
        dy = -newPos.Y();
    else if (fModuleSizeY - newPos.Y() < 0)
        dy = fModuleSizeY - newPos.Y();

    TVector2 dist = TVector2(dx, dy);
    return dist;
}
///////////////////////////////////////////////
/// \brief Returns the pixel origin (left-bottom) position for a given *channel*
/// and *pixel* indexes.
///
TVector2 TRestReadoutModule::GetPixelOrigin(Int_t channel, Int_t pixel) {
    return GetPixelVertex(channel, pixel, 0);
}

///////////////////////////////////////////////
/// \brief Returns any of the pixel vertex position for a given *channel* and
/// *pixel* indexes.
///
/// \param vertex A value between 0-3 definning the vertex position to be
/// returned
///
TVector2 TRestReadoutModule::GetPixelVertex(Int_t channel, Int_t pixel, Int_t vertex) {
    TVector2 pixPosition = GetChannel(channel)->GetPixel(pixel)->GetVertex(vertex);

    pixPosition = pixPosition.Rotate(fModuleRotation * TMath::Pi() / 180.);
    pixPosition = pixPosition + TVector2(fModuleOriginX, fModuleOriginY);
    return pixPosition;
}

///////////////////////////////////////////////
/// \brief Returns the center pixel position for a given *channel* and
/// *pixel* indexes.
///
/// \param vertex A value between 0-3 definning the vertex position to be
/// returned
///
TVector2 TRestReadoutModule::GetPixelCenter(Int_t channel, Int_t pixel) {
    TVector2 pixCenter = GetChannel(channel)->GetPixel(pixel)->GetCenter();

    pixCenter = pixCenter.Rotate(fModuleRotation * TMath::Pi() / 180.);
    pixCenter = pixCenter + TVector2(fModuleOriginX, fModuleOriginY);
    return pixCenter;
}

///////////////////////////////////////////////
/// \brief Returns the pixel type for a given *channel* and
/// *pixel* indexes.
///
/// \param vertex A boolean that is true if the pixel is triangular, false
/// otherwise
///
Bool_t TRestReadoutModule::GetPixelTriangle(Int_t channel, Int_t pixel) {
    Bool_t type = GetChannel(channel)->GetPixel(pixel)->GetTriangle();

    return type;
}

TVector2 TRestReadoutModule::GetPixelOrigin(TRestReadoutPixel* pix) { return GetPixelVertex(pix, 0); }

TVector2 TRestReadoutModule::GetPixelVertex(TRestReadoutPixel* pix, Int_t vertex) {
    TVector2 pixPosition = pix->GetVertex(vertex);
    pixPosition = pixPosition.Rotate(fModuleRotation * TMath::Pi() / 180.);
    pixPosition = pixPosition + TVector2(fModuleOriginX, fModuleOriginY);
    return pixPosition;
}

TVector2 TRestReadoutModule::GetPixelCenter(TRestReadoutPixel* pix) {
    TVector2 corner1(GetPixelVertex(pix, 0));
    TVector2 corner2(GetPixelVertex(pix, 2));
    TVector2 center = (corner1 + corner2) / 2.;
    return center;
}

Bool_t TRestReadoutModule::GetPixelTriangle(TRestReadoutPixel* pix) { return pix->GetTriangle(); }

///////////////////////////////////////////////
/// \brief Returns the coordinates of the specified vertex index *n*. The
/// physical coordinates relative to the readout plane are returned, including
/// rotation.
///
/// \param n A value between 0-3 definning the vertex position to be returned
///
TVector2 TRestReadoutModule::GetVertex(int n) const {
    TVector2 vertex(0, 0);
    TVector2 origin(fModuleOriginX, fModuleOriginY);

    if (n % 4 == 0)
        return origin;
    else if (n % 4 == 1) {
        vertex.Set(fModuleSizeX, 0);
        vertex = vertex.Rotate(fModuleRotation * TMath::Pi() / 180.);

        vertex = vertex + origin;
    } else if (n % 4 == 2) {
        vertex.Set(fModuleSizeX, fModuleSizeY);
        vertex = vertex.Rotate(fModuleRotation * TMath::Pi() / 180.);

        vertex = vertex + origin;
    } else if (n % 4 == 3) {
        vertex.Set(0, fModuleSizeY);
        vertex = vertex.Rotate(fModuleRotation * TMath::Pi() / 180.);

        vertex = vertex + origin;
    }
    return vertex;
}

///////////////////////////////////////////////
/// \brief Adds a new channel to the module
///
void TRestReadoutModule::AddChannel(TRestReadoutChannel& rChannel) {
    for (int i = 0; i < rChannel.GetNumberOfPixels(); i++) {
        // TODO we expect here that the user will only do pixel rotations between 0
        // and 90 degrees, we must force that on pixel definition or fix it here
        Double_t oX = rChannel.GetPixel(i)->GetVertex(3).X();
        Double_t oY = rChannel.GetPixel(i)->GetVertex(3).Y();
        Double_t sX = rChannel.GetPixel(i)->GetVertex(1).X();
        Double_t sY = rChannel.GetPixel(i)->GetVertex(1).Y();

        if (oX + fTolerance < 0 || oY + fTolerance < 0 || sX - fTolerance > fModuleSizeX ||
            sY - fTolerance > fModuleSizeY) {
            if (showWarnings) {
                cout << "REST Warning (AddChannel) pixel outside the module boundaries" << endl;
                cout << "Channel: " << fReadoutChannel.size() << ", Pixel : " << i << endl;
                cout << "Pixel origin = (" << oX << " , " << oY << ")" << endl;
                cout << "Pixel size = (" << sX << " , " << sY << ")" << endl;
                cout << "Module size = (" << fModuleSizeX << " , " << fModuleSizeY << ")" << endl;
            }
        }
    }

    fReadoutChannel.push_back(rChannel);
}

///////////////////////////////////////////////
/// \brief Not implemented
///
void TRestReadoutModule::Draw() {}

///////////////////////////////////////////////
/// \brief Prints the module details and channels if *fullDetail* is enabled.
///
void TRestReadoutModule::Print(Int_t DetailLevel) {
    if (DetailLevel >= 0) {
        RESTMetadata << "-- Readout module : " << GetModuleID() << RESTendl;
        RESTMetadata << "----------------------------------------------------------------" << RESTendl;
        RESTMetadata << "-- Origin position : X = " << fModuleOriginX << " mm "
                     << " Y : " << fModuleOriginY << " mm" << RESTendl;
        RESTMetadata << "-- Size : X = " << fModuleSizeX << " Y : " << fModuleSizeY << RESTendl;
        RESTMetadata << "-- Rotation : " << fModuleRotation << " degrees" << RESTendl;
        RESTMetadata << "-- Total channels : " << GetNumberOfChannels() << RESTendl;
        RESTMetadata << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << RESTendl;

        for (int n = 0; n < GetNumberOfChannels(); n++) fReadoutChannel[n].Print(DetailLevel - 1);
    }
}

void TRestReadoutModule::PrintDecoding() {
    cout << "daqChannel\tphysicalChannel" << endl;

    map<int, int> channelmap;
    for (int i = 0; i < GetNumberOfChannels(); i++) {
        TRestReadoutChannel* chn = &fReadoutChannel[i];
        channelmap[chn->GetDaqID()] = i;
    }
    for (auto iter : channelmap) {
        cout << iter.first << "\t" << iter.second << endl;
    }
}