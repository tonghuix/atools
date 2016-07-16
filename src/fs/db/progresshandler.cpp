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

#include "fs/db/progresshandler.h"
#include "fs/navdatabaseoptions.h"
#include "fs/scenery/sceneryarea.h"

#include <QDebug>

namespace atools {
namespace fs {
namespace db {

ProgressHandler::ProgressHandler(const NavDatabaseOptions *options)
{
  if(options->getProgressCallback() != nullptr)
    handler = options->getProgressCallback();
}

void ProgressHandler::increaseCurrent(int increase)
{
  info.current += increase;
}

bool ProgressHandler::reportOtherMsg(const QString& otherAction)
{
  info.otherAction = otherAction;
  info.newFile = false;
  info.newSceneryArea = false;
  info.newOther = true;

  return callHandler();
}

bool ProgressHandler::reportOther(const QString& otherAction, int current, bool silent)
{
  if(current != -1)
    info.current = current;
  else
    info.current++;
  info.otherAction = otherAction;

  info.newFile = false;
  info.newSceneryArea = false;
  info.newOther = true;

  if(silent)
    return false;
  else
    return callHandler();
}

bool ProgressHandler::report(const QString& bglFilepath, int current)
{
  if(current != -1)
    info.current = current;
  else
    info.current++;
  info.bglFilename = bglFilepath;

  info.newFile = true;
  info.newSceneryArea = false;
  info.newOther = false;

  return callHandler();
}

bool ProgressHandler::reportFinish()
{
  info.lastCall = true;
  info.newFile = false;
  info.newSceneryArea = false;
  info.newOther = false;

  return callHandler();
}

void ProgressHandler::setTotal(int total)
{
  info.total = total;
}

void ProgressHandler::reset()
{
  info.current = 0;
  info.sceneryArea = nullptr;
  info.bglFilename.clear();
  info.newFile = false;
  info.newSceneryArea = false;
  info.newOther = false;
  info.firstCall = true;
  info.lastCall = false;
}

bool ProgressHandler::report(const scenery::SceneryArea *sceneryArea, int current)
{
  if(current != -1)
    info.current = current;
  else
    info.current++;
  info.sceneryArea = sceneryArea;

  info.newFile = false;
  info.newSceneryArea = true;
  info.newOther = false;

  return callHandler();
}

bool ProgressHandler::callHandler()
{
  bool retval = false;

  // Alway call default handler - this one cannot call cancel
  defaultHandler(info);

  if(handler != nullptr)
    // Call user handler
    retval = handler(info);

  if(info.firstCall)
    info.firstCall = false;

  return retval;
}

/*
 * Default handler prints to console or log only
 */
void ProgressHandler::defaultHandler(const atools::fs::NavDatabaseProgress& inf)
{
  if(inf.isNewFile())
    qInfo() << "====" << numbersAsString(inf) << inf.getBglFilename();

  if(inf.isNewSceneryArea())
  {
    qInfo() << "======================================================================";
    qInfo() << "==========" << numbersAsString(inf) << inf.getSceneryTitle();
    qInfo() << "==========" << inf.getSceneryPath();
  }

  if(inf.isNewOther())
    qInfo() << "====" << numbersAsString(inf) << inf.getOtherAction();
}

QString ProgressHandler::numbersAsString(const atools::fs::NavDatabaseProgress& inf)
{
  return QString("%1 of %2 (%3 %)").arg(inf.current).arg(inf.total).arg(100 * info.current / info.total);
}

} // namespace writer
} // namespace fs
} // namespace atools
