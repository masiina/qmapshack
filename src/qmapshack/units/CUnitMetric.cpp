/**********************************************************************************************
    Copyright (C) 2008 Oliver Eichler <oliver.eichler@gmx.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 USA

**********************************************************************************************/
#include "units/CUnitMetric.h"

CUnitMetric::CUnitMetric(QObject* parent) : IUnit(eTypeMetric, "m", 1.0, "km/h", 3.6, "m", 1.0, parent) {}

void CUnitMetric::meter2distance(qreal meter, QString& val, QString& unit) const /* override */
{
  if (meter == NOFLOAT) {
    val = "-";
    unit.clear();
  } else if (meter < 10) {
    val = QString::asprintf("%1.1f", meter);
    unit = "m";
  } else if (meter < 1000) {
    val = QString::asprintf("%1.0f", meter);
    unit = "m";
  } else if (meter < 10000) {
    val = QString::asprintf("%1.2f", meter / 1000);
    unit = "km";
  } else if (meter < 20000) {
    val = QString::asprintf("%1.1f", meter / 1000);
    unit = "km";
  } else {
    val = QString::asprintf("%1.0f", meter / 1000);
    unit = "km";
  }
}

void CUnitMetric::meter2distance(qreal meter, qreal& val, QString& unit) const /* override */
{
  if (meter == NOFLOAT) {
    val = NOFLOAT;
    unit.clear();
  } else if (meter < 1000) {
    val = meter;
    unit = "m";
  } else {
    val = meter / 1000;
    unit = "km";
  }
}

void CUnitMetric::meter2speed(qreal meter, QString& val, QString& unit) const /* override */
{
  if (meter == NOFLOAT) {
    val = "-";
    unit.clear();
  } else if (meter < 0.27) {
    val = QString::asprintf("%1.0f", meter * speedFactor * 1000);
    unit = "m/h";
  } else if (meter < 10.0) {
    val = QString::asprintf("%1.1f", meter * speedFactor);
    unit = speedUnit;
  } else {
    val = QString::asprintf("%1.0f", meter * speedFactor);
    unit = speedUnit;
  }
}

void CUnitMetric::meter2speed(qreal meter, qreal& val, QString& unit) const /* override */
{
  if (meter == NOFLOAT) {
    val = NOFLOAT;
    unit.clear();
  } else if (meter < 0.27) {
    val = meter * speedFactor * 1000;
    unit = "m/h";
  } else {
    val = meter * speedFactor;
    unit = speedUnit;
  }
}

void CUnitMetric::meter2area(qreal meter, QString& val, QString& unit) const /* override */
{
  if (meter == NOFLOAT) {
    val = "-";
    unit.clear();
  } else if (meter < 100000) {
    val = QString::asprintf("%1.0f", meter);
    unit = "m²";
  } else {
    val = QString::asprintf("%1.2f", meter / 1000000);
    unit = "km²";
  }
}

void CUnitMetric::meter2area(qreal meter, qreal& val, QString& unit) const /* override */
{
  if (meter == NOFLOAT) {
    val = NOFLOAT;
    unit.clear();
  } else if (meter < 100000) {
    val = meter;
    unit = "m²";
  } else {
    val = meter / 1000000;
    unit = "km²";
  }
}

qreal CUnitMetric::elevation2meter(const QString& val) const /* override */
{
  return val.toDouble();
}

void CUnitMetric::meter2unit(qreal meter, qreal& scale, QString& unit) const {
  if (meter > 1000) {
    scale = 0.001;
    unit = "km";
  } else {
    scale = 1.0;
    unit = "m";
  }
}
