//=============================================================================
//  MuseScore
//  Music Composition & Notation
//  $Id: palette.cpp 5576 2012-04-24 19:15:22Z wschweer $
//
//  Copyright (C) 2011 Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENSE.GPL
//=============================================================================

#ifndef __PLUGIN_MANAGER_H__
#define __PLUGIN_MANAGER_H__

#include "ui_pluginManager.h"
#include "preferences.h"
#include <QtSql>
#include <QtDebug>

namespace Ms {

class Shortcut;

//---------------------------------------------------------
//   PluginManager
//---------------------------------------------------------

class PluginManager : public QDialog, public Ui::PluginManager {
      Q_OBJECT

      QMap<QString, Shortcut*> localShortcuts;
      bool shortcutsChanged;
      Preferences prefs;

      void readSettings();
      void loadList(bool forceRefresh);

      virtual void closeEvent(QCloseEvent*);
      virtual void accept();

   private:
      void FillTable();

   public:
      /**QSqlDatabase db;
      void connClose(){
          db.close();
          db.removeDatabase(QSqlDatabase::defaultConnection);
      }
      bool connOpen(){
          db = QSqlDatabase::addDatabase("QMYSQL");
          db.setHostName("localhost");
          db.setUserName("root");
          db.setPassword("");
          db.setDatabaseName("plugin_manager");
          db.open();

          if (!db.open())
              qDebug()<<("Faild");
          else
              qDebug()<<("Connected");
          return (db.open());
      }**/


   private slots:
      void definePluginShortcutClicked();
      void clearPluginShortcutClicked();
      void pluginListItemChanged(QListWidgetItem*, QListWidgetItem*);
      void allPluginsItemChanged(QListWidgetItem*, QListWidgetItem*);
      void pluginLoadToggled(QListWidgetItem*);
      void reloadPluginsClicked();

      void on_pushButton_clicked();

      void on_pushButton_2_clicked();


      void on_pushButton_3_clicked();

      void on_AutoUpdate_btn_clicked();

signals:
      void closed(bool);

   public:
      PluginManager(QWidget* parent = 0);
      void writeSettings();
      void init();
      };


} // namespace Ms
#endif

