/*****************************************************************************
* Copyright 2015-2016 Alexander Barthel albar965@mailbox.org
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

#include "fs/writer/ap/rw/runwaywriter.h"
#include "fs/writer//datawriter.h"
#include "fs/bgl/util.h"
#include "fs/writer/ap/airportwriter.h"
#include "fs/writer/ap/rw/runwayendwriter.h"
#include "fs/writer/runwayindex.h"
#include "fs/bglreaderoptions.h"

#include <QString>

namespace atools {
namespace fs {
namespace writer {

using atools::fs::bgl::Runway;
using atools::sql::SqlQuery;

void RunwayWriter::writeObject(const Runway *type)
{
  int runwayId = getNextId();

  QString apIdent = getDataWriter().getAirportWriter()->getCurrentAirportIdent();

  RunwayEndWriter *rweWriter = getDataWriter().getRunwayEndWriter();
  rweWriter->writeOne(&(type->getPrimary()));
  int primaryEndId = rweWriter->getCurrentId();
  getRunwayIndex()->add(apIdent, type->getPrimary().getName(), primaryEndId);

  rweWriter->writeOne(&(type->getSecondary()));
  int secondaryEndId = rweWriter->getCurrentId();
  getRunwayIndex()->add(apIdent, type->getSecondary().getName(), secondaryEndId);

  if(getOptions().isVerbose())
    qDebug() << "Writing Runway for airport " << apIdent;

  bind(":runway_id", runwayId);
  bind(":airport_id", getDataWriter().getAirportWriter()->getCurrentId());
  bind(":primary_end_id", primaryEndId);
  bind(":secondary_end_id", secondaryEndId);
  bind(":surface", Runway::surfaceToStr(type->getSurface()));
  bind(":length", bgl::util::meterToFeet(type->getLength()));
  bind(":width", bgl::util::meterToFeet(type->getWidth()));
  bind(":heading", type->getHeading());
  bind(":pattern_altitude", bgl::util::meterToFeet(type->getPatternAltitude(), 1));
  bind(":marking_flags", type->getMarkingFlags());
  bind(":light_flags", type->getLightFlags());
  bind(":pattern_flags", type->getPatternFlags());
  bind(":edge_light", bgl::util::enumToStr(Runway::lightToStr, type->getEdgeLight()));
  bind(":center_light",
       bgl::util::enumToStr(Runway::lightToStr, type->getCenterLight()));
  bind(":has_center_red", type->isCenterRed());
  bind(":altitude", bgl::util::meterToFeet(type->getPosition().getAltitude()));
  bind(":lonx", type->getPosition().getLonX());
  bind(":laty", type->getPosition().getLatY());

  executeStatement();
}

} // namespace writer
} // namespace fs
} // namespace atools
