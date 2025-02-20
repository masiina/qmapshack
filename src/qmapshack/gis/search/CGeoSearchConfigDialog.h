/**********************************************************************************************
    Copyright (C) 2018 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#ifndef CGEOSEARCHCONFIGDIALOG_H
#define CGEOSEARCHCONFIGDIALOG_H

#include "ui_IGeoSearchConfigDialog.h"

class CGeoSearchConfig;

class CGeoSearchConfigDialog : public QDialog, Ui_IGeoSearchConfigDialog {
 public:
  CGeoSearchConfigDialog(QWidget* parent, CGeoSearchConfig* searchConfig);

 private slots:
  void slotAccepted();

 private:
  CGeoSearchConfig* searchConfig;
};

#endif
