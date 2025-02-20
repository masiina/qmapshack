/**********************************************************************************************
    Copyright (C) 2017 Michel Durand <zero@cms123.fr>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#include "gis/suunto/CLogProject.h"

#include <QtWidgets>

#include "CMainWindow.h"
#include "gis/CGisListWks.h"
#include "gis/suunto/ISuuntoProject.h"
#include "gis/trk/CGisItemTrk.h"

const QList<extension_t> CLogProject::extensions = {
    {"Latitude", 0.0000001, 0.0, ASSIGN_VALUE(lat, NIL)}  // unit [°]
    ,
    {"Longitude", 0.0000001, 0.0, ASSIGN_VALUE(lon, NIL)}  // unit [°]
    ,
    {"Altitude", 1.0, 0.0, ASSIGN_VALUE(ele, NIL)}  // unit [m]
    ,
    {"VerticalSpeed", 0.01, 0.0, ASSIGN_VALUE(extensions["gpxdata:verticalSpeed"], NIL)}  // unit [m/h]
    ,
    {"HR", 1.0, 0.0, ASSIGN_VALUE(extensions["gpxtpx:TrackPointExtension|gpxtpx:hr"], qRound)}  // unit [bpm]
    ,
    {"Cadence", 1.0, 0.0, ASSIGN_VALUE(extensions["gpxdata:cadence"], NIL)}  // unit [bpm]
    ,
    {"Temperature", 0.1, 0.0, ASSIGN_VALUE(extensions["gpxdata:temp"], NIL)}  // unit [°C]
    ,
    {"SeaLevelPressure", 0.1, 0.0, ASSIGN_VALUE(extensions["gpxdata:seaLevelPressure"], NIL)}  // unit [hPa]
    ,
    {"Speed", 0.01, 0.0, ASSIGN_VALUE(extensions["gpxdata:speed"], NIL)}  // unit [m/s]
    ,
    {"EnergyConsumption", 0.1, 0.0, ASSIGN_VALUE(extensions["gpxdata:energy"], NIL)}  // unit [kCal/min]
};

CLogProject::CLogProject(const QString& filename, CGisListWks* parent) : ISuuntoProject(eTypeLog, filename, parent) {
  setIcon(CGisListWks::eColumnIcon, QIcon("://icons/32x32/LogProject.png"));
  blockUpdateItems(true);
  loadLog(filename);
  blockUpdateItems(false);
  setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
}

void CLogProject::loadLog(const QString& filename) {
  try {
    loadLog(filename, this);
  } catch (QString& errormsg) {
    QMessageBox::critical(CMainWindow::getBestWidgetForParent(), tr("Failed to load file %1...").arg(filename),
                          errormsg, QMessageBox::Abort);
    valid = false;
  }
}

void CLogProject::loadLog(const QString& filename, CLogProject* project) {
  QFile file(filename);

  // if the file does not exist, the filename is assumed to be a name for a new project
  if (!file.exists() || QFileInfo(filename).suffix().toLower() != "log") {
    project->filename.clear();
    project->setupName(filename);
    project->setToolTip(CGisListWks::eColumnName, project->getInfo());
    project->valid = true;
    return;
  }

  if (!file.open(QIODevice::ReadOnly)) {
    throw tr("Failed to open %1").arg(filename);
  }

  // load file content to xml document
  QDomDocument xml;
  QString msg;
  int line;
  int column;
  if (!xml.setContent(&file, false, &msg, &line, &column)) {
    file.close();
    throw tr("Failed to read: %1\nline %2, column %3:\n %4").arg(filename).arg(line).arg(column).arg(msg);
  }
  file.close();

  QDomElement xmlOpenambitlog = xml.documentElement();
  if (xmlOpenambitlog.tagName() != "openambitlog") {
    throw tr("Not an Openambit log file: %1").arg(filename);
  }

  CTrackData trk;

  if (xmlOpenambitlog.namedItem("DeviceInfo").isElement()) {
    const QDomNode& xmlDeviceInfo = xmlOpenambitlog.namedItem("DeviceInfo");
    if (xmlDeviceInfo.namedItem("Name").isElement()) {
      trk.cmt = tr("Device: %1<br/>").arg(xmlDeviceInfo.namedItem("Name").toElement().text());
    }
  }

  if (xmlOpenambitlog.namedItem("Log").isElement()) {
    QDateTime time0;  // start time of the track

    const QDomNode& xmlLog = xmlOpenambitlog.namedItem("Log");
    if (xmlLog.namedItem("Header").isElement()) {
      const QDomNode& xmlHeader = xmlLog.namedItem("Header");

      if (xmlHeader.namedItem("DateTime").isElement()) {
        QString dateTimeStr = xmlHeader.namedItem("DateTime").toElement().text();
        trk.name = dateTimeStr;                     // date of beginning of recording is chosen as track name
        IUnit::parseTimestamp(dateTimeStr, time0);  // as local time
      }

      if (xmlHeader.namedItem("Activity").isElement()) {
        trk.desc = xmlHeader.namedItem("Activity").toElement().text();
      }

      if (xmlHeader.namedItem("RecoveryTime").isElement()) {
        trk.cmt += tr("Recovery time: %1 h<br/>")
                       .arg(xmlHeader.namedItem("RecoveryTime").toElement().text().toInt() / 3600000);
      }

      if (xmlHeader.namedItem("PeakTrainingEffect").isElement()) {
        trk.cmt += tr("Peak Training Effect: %1<br/>")
                       .arg(xmlHeader.namedItem("PeakTrainingEffect").toElement().text().toDouble() / 10.0);
      }

      if (xmlHeader.namedItem("Energy").isElement()) {
        trk.cmt += tr("Energy: %1 kCal<br/>").arg((int)xmlHeader.namedItem("Energy").toElement().text().toDouble());
      }
    }

    if (xmlLog.namedItem("Samples").isElement()) {
      const QDomNode& xmlSamples = xmlLog.namedItem("Samples");
      const QDomNodeList& xmlSampleList = xmlSamples.toElement().elementsByTagName("Sample");

      if (xmlSampleList.count() > 0) {
        bool UTCtimeFound = false;
        for (int i = 0; i < xmlSampleList.count(); i++)  // browse XML samples
        {                                                // look for samples with UTC timestamp
          const QDomNode& xmlSample = xmlSampleList.item(i);

          if (xmlSample.namedItem("UTCReference").isElement()) {
            QString timeStr = xmlSample.namedItem("UTCReference").toElement().text();

            if (xmlSample.namedItem("Time").isElement()) {
              IUnit::parseTimestamp(timeStr, time0);
              time0 = time0.addMSecs(-xmlSample.namedItem("Time")
                                          .toElement()
                                          .text()
                                          .toDouble());  // substract current sample time to get start time
              UTCtimeFound = true;
              break;
            }
          }
        }

        if (!UTCtimeFound) {
          QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Use of local time..."),
                               tr("No UTC time has been found in file %1. "
                                  "Local computer time will be used. "
                                  "You can adjust time using a time filter if needed.")
                                   .arg(filename),
                               QMessageBox::Ok);
        }

        bool sampleWithPositionFound = false;
        QList<sample_t> samplesList;
        QList<QDateTime> lapsList;

        for (int i = 0; i < xmlSampleList.count(); i++)  // browse XML samples
        {
          sample_t sample;
          const QDomNode& xmlSample = xmlSampleList.item(i);

          if (xmlSample.namedItem("Latitude").isElement()) {
            sampleWithPositionFound = true;
          }

          if (xmlSample.namedItem("Time").isElement()) {
            sample.time = time0.addMSecs(xmlSample.namedItem("Time").toElement().text().toDouble());
          }

          if (xmlSample.namedItem("Type").isElement()) {
            if (xmlSample.namedItem("Type").toElement().text() == "lap-info") {
              if (xmlSample.namedItem("Lap").isElement()) {
                const QDomNode& xmlLap = xmlSample.namedItem("Lap");
                if (xmlLap.namedItem("Type").isElement()) {
                  if (xmlLap.namedItem("Type").toElement().text() == "Manual") {
                    lapsList
                        << sample.time;  // stores timestamps of the samples where the the "Lap" button has been pressed
                  }
                }
              }
            } else if (xmlSample.namedItem("Type").toElement().text() == "gps-small" ||
                       xmlSample.namedItem("Type").toElement().text() == "gps-base" ||
                       xmlSample.namedItem("Type").toElement().text() == "gps-tiny" ||
                       xmlSample.namedItem("Type").toElement().text() == "position" ||
                       xmlSample.namedItem("Type").toElement().text() == "periodic") {
              for (const extension_t& ext : extensions) {
                if (xmlSample.namedItem(ext.tag).isElement()) {
                  const QDomNode& xmlSampleData = xmlSample.namedItem(ext.tag);
                  sample[ext.tag] = xmlSampleData.toElement().text().toDouble() * ext.scale + ext.offset;
                }
              }
              samplesList << sample;
            }
          }
        }

        if (!sampleWithPositionFound) {
          throw tr("This LOG file does not contain any position data and can not be displayed by QMapShack: %1")
              .arg(filename);
        }

        fillTrackPointsFromSamples(samplesList, lapsList, trk, extensions);

        new CGisItemTrk(trk, project);

        project->sortItems();
        project->setupName(QFileInfo(filename).completeBaseName().replace("_", " "));
        project->setToolTip(CGisListWks::eColumnName, project->getInfo());
        project->valid = true;
      }
    }
  }
}
