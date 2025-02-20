/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CCANVASSELECT_H
#define CCANVASSELECT_H

#include "ui_ICanvasSelect.h"
class CCanvas;

class CCanvasSelect : public QDialog, private Ui::ICanvasSelect {
  Q_OBJECT
 public:
  CCanvasSelect(CCanvas*& canvas, QWidget* parent);
  virtual ~CCanvasSelect() = default;

 public slots:
  int exec() override;
  void accept() override;

 private:
  CCanvas*& canvas;
};

#endif  // CCANVASSELECT_H
