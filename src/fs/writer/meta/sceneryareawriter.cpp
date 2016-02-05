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

#include "fs/writer/meta/sceneryareawriter.h"
#include "fs/writer/datawriter.h"
#include "fs/bglreaderoptions.h"

#include "logging/loggingdefs.h"

#include <QDir>

namespace atools {
namespace fs {
namespace writer {

using scenery::SceneryArea;
using atools::sql::SqlQuery;

void SceneryAreaWriter::writeObject(const SceneryArea *type)
{
  if(getOptions().isVerbose())
    qDebug() << "Writing SceneryArea layer " << type->getLayer() << " title " << type->getTitle();

  bind(":scenery_area_id", getNextId());
  bind(":number", type->getAreaNumber());
  bind(":layer", type->getLayer());
  bind(":title", type->getTitle());
  bind(":remote_path", QDir::toNativeSeparators(type->getRemotePath()));
  bind(":local_path", QDir::toNativeSeparators(type->getLocalPath()));
  bind(":active", type->isActive());
  bind(":required", type->isRequired());
  bind(":exclude", type->getExclude());

  executeStatement();
}

} // namespace writer
} // namespace fs
} // namespace atools