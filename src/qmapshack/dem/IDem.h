/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>
                  2019 Johannes Zellner johannes@zellner.org

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

#ifndef IDEM_H
#define IDEM_H

#include <QObject>
#include <QPointer>

#include "canvas/IDrawContext.h"
#include "canvas/IDrawObject.h"
#include "gis/proj_x.h"

#define CUSTOM_SLOPE_COLORTABLE (-1)

class CDemDraw;
class IDemProp;
class QSettings;

struct SlopePresets {
  const char* name;
  const qreal steps[5];
};

class IDem : public IDrawObject {
  Q_OBJECT
 public:
  IDem(CDemDraw* parent);
  virtual ~IDem();

  void saveConfig(QSettings& cfg) override;

  void loadConfig(QSettings& cfg) override;

  virtual void draw(IDrawContext::buffer_t& buf) = 0;

  virtual qreal getElevationAt(const QPointF& pos, bool checkScale) = 0;
  virtual qreal getSlopeAt(const QPointF& pos, bool checkScale) = 0;

  bool activated() const { return isActivated; }

  /**
     @brief Get the dem's setup widget.

     As default an instance of CDemPropSetup is used. For other setups you have
     to override this method.

     @return A pointer to the widget. Use a smart pointer to store as the widget can be destroyed at any time
   */
  virtual IDemProp* getSetup();

  bool doHillshading() const { return bHillshading; }

  int getFactorHillshading() const;

  bool doSlopeShading() const { return bSlopeShading; }

  int getFactorSlopeShading() const;

  bool doSlopeColor() const { return bSlopeColor; }

  bool doElevationLimit() const { return bElevationLimit; }

  int getElevationLimit() const { return elevationValue; }

  bool doElevationShading() const { return bElevationShading; }

  int getElevationShadeLimitLow() const { return elevationShadeLimitLow; }

  int getElevationShadeLimitHi() const { return elevationShadeLimitHi; }

  const QVector<QRgb> getSlopeColorTable() const { return slopetable; }

  bool doShowElevationShadeScale() const { return bShowElevationShadeScale; }

  static const struct SlopePresets slopePresets[7];
  static const size_t slopePresetCount = sizeof(IDem::slopePresets) / sizeof(IDem::slopePresets[0]);

  const qreal* getCurrentSlopeStepTable() const;

  int getSlopeStepTableIndex() const { return gradeSlopeColor; }

  void setSlopeStepTable(int idx);
  void setSlopeStepTableCustomValue(int idx, int val);
  void setElevationLimit(int val);

  void initElevationShadeTable();
  void setElevationShadeLow(int val);
  void setElevationShadeHi(int val);

  enum winsize_e { eWinsize3x3 = 9, eWinsize4x4 = 16 };

 public slots:
  void slotSetHillshading(bool yes) { bHillshading = yes; }

  void slotSetFactorHillshade(int f);

  void slotSetSlopeShading(bool yes) { bSlopeShading = yes; }

  void slotSetFactorSlopeShade(int f);

  void slotSetSlopeColor(bool yes) { bSlopeColor = yes; }

  void slotSetElevationLimit(bool yes) { bElevationLimit = yes; }

  void slotSetElevationShading(bool yes) { bElevationShading = yes; }

  void slotShowElevationShadeScale(bool yes);

 protected:
  void hillshading(QVector<float>& data, qreal w, qreal h, QImage& img) const;

  void slopeShading(QVector<float>& data, qreal w, qreal h, QImage& img) const;

  void slopecolor(QVector<float>& data, qreal w, qreal h, QImage& img) const;

  void elevationLimit(QVector<float>& data, qreal w, qreal h, QImage& img) const;

  void elevationShading(QVector<float>& data, qreal w, qreal h, QImage& img) const;

  /**
     @brief Slope in degrees based on a window. Origin is at point (1,1), counting from zero.
     @param win2  window data
     @param size  size of window (eWinsize3x3 or eWinsize4x4)
     @param x     Fractional value (0..1) for interpolation in x (4x4 window only)
     @param y     Fractional value (0..1) for interpolation in y (4x4 window only)
     @return      Slope in degrees
   */
  qreal slopeOfWindowInterp(float* win2, winsize_e size, qreal x, qreal y) const;

  /**
     @brief Reproject (translate, rotate, scale) tile before drawing it.
     @param img   the tile as QImage
     @param l     a 4 point polygon to fit the tile in
     @param p     the QPainter used to paint the tile
   */
  void drawTile(QImage& img, QPolygonF& l, QPainter& p) const;

  CDemDraw* dem;

  /**
      target should always be "EPSG:4326"
      source will be the map's projection
   */
  CProj proj;

  /// width in number of px
  qint32 xsize_px = 0;
  /// height in number of px
  qint32 ysize_px = 0;

  /// scale [px/m]
  qreal xscale = 1.0;
  /// scale [px/m]
  qreal yscale = 1.0;

  /**
     @brief True if map was loaded successfully
   */
  bool isActivated = false;

  /// the setup dialog. Use getSetup() for access
  QPointer<IDemProp> setup;

  QVector<QRgb> graytable;

  QVector<QRgb> elevationShadeTable;

  QVector<QRgb> slopetable;

  QVector<QRgb> elevationtable;

  int hasNoData = 0;

  double noData = 0;

 private:
  bool bHillshading = false;
  qreal factorHillshading = 0.1666666716337204;
  bool bSlopeShading = false;
  qreal factorSlopeShading = 1.0;
  bool bSlopeColor = false;
  bool bElevationLimit = false;
  int gradeSlopeColor = 0;
  qreal slopeCustomStepTable[5];
  int elevationValue;
  bool bElevationShading = false;
  int elevationShadeLimitLow;
  int elevationShadeLimitHi;
  bool bShowElevationShadeScale = false;
};

#endif  // IDEM_H
