/*****************************************************************************
* Copyright 2015-2020 Alexander Barthel alex@littlenavmap.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "fs/sc/simconnectaircraft.h"

#include <QDebug>
#include <QDataStream>

namespace atools {
namespace fs {
namespace sc {

SimConnectAircraft::SimConnectAircraft()
{

}

SimConnectAircraft::SimConnectAircraft(const SimConnectAircraft& other)
{
  *this = other;
}

void SimConnectAircraft::read(QDataStream& in)
{
  in >> objectId;

  quint16 intFlags;
  in >> intFlags;
  flags = AircraftFlags(intFlags);

  readString(in, airplaneTitle);
  readString(in, airplaneModel);
  readString(in, airplaneReg);
  readString(in, airplaneType);
  readString(in, airplaneAirline);
  readString(in, airplaneFlightnumber);
  readString(in, fromIdent);
  readString(in, toIdent);

  float lonx, laty, altitude;
  quint8 categoryByte, engineTypeByte;

  in >> lonx >> laty >> altitude >> headingTrueDeg >> headingMagDeg
  >> groundSpeedKts >> indicatedSpeedKts >> verticalSpeedFeetPerMin
  >> indicatedAltitudeFt >> trueAirspeedKts >> machSpeed
  >> numberOfEngines >> wingSpanFt >> modelRadiusFt >> deckHeight >> categoryByte >> engineTypeByte;

  position.setAltitude(altitude);
  position.setLonX(lonx);
  position.setLatY(laty);

  category = static_cast<Category>(categoryByte);
  engineType = static_cast<EngineType>(engineTypeByte);
}

int SimConnectAircraft::getModelRadiusCorrected() const
{
  if(modelRadiusFt > 0)
    return modelRadiusFt;
  else
  {
    if(category == CARRIER)
      return 500;
    else if(category == FRIGATE)
      return 160;
    else
    {
      switch(engineType)
      {
        case PISTON:
        case NO_ENGINE:
        case HELO_TURBINE:
        case UNSUPPORTED:
          return 20;

        case JET:
          return 60;

        case TURBOPROP:
          return 40;
      }
    }
    return 20;
  }
}

void SimConnectAircraft::write(QDataStream& out) const
{
  out << objectId << static_cast<quint16>(flags);

  writeString(out, airplaneTitle);
  writeString(out, airplaneModel);
  writeString(out, airplaneReg);
  writeString(out, airplaneType);
  writeString(out, airplaneAirline);
  writeString(out, airplaneFlightnumber);
  writeString(out, fromIdent);
  writeString(out, toIdent);

  out << position.getLonX() << position.getLatY() << position.getAltitude() << headingTrueDeg << headingMagDeg
      << groundSpeedKts << indicatedSpeedKts << verticalSpeedFeetPerMin
      << indicatedAltitudeFt << trueAirspeedKts << machSpeed
      << numberOfEngines << wingSpanFt << modelRadiusFt << deckHeight
      << static_cast<quint8>(category) << static_cast<quint8>(engineType);
}

bool SimConnectAircraft::isSameAircraft(const SimConnectAircraft& other) const
{
  return airplaneTitle == other.airplaneTitle &&
         airplaneModel == other.airplaneModel &&
         airplaneReg == other.airplaneReg &&
         airplaneType == other.airplaneType &&
         airplaneAirline == other.airplaneAirline &&
         airplaneFlightnumber == other.airplaneFlightnumber;
}

void SimConnectAircraft::updateAircraftNames(const QString& airplaneTypeParam, const QString& airplaneAirlineParam,
                                             const QString& airplaneTitleParam, const QString& airplaneModelParam)
{
  airplaneType = airplaneTypeParam;
  airplaneAirline = airplaneAirlineParam;
  airplaneTitle = airplaneTitleParam;
  airplaneModel = airplaneModelParam;
}

} // namespace sc
} // namespace fs
} // namespace atools
