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

#include "pluginManager.h"
#include "shortcutcapturedialog.h"
#include "musescore.h"
#include <QDesktopServices>
#include <QUrl>

#include <iostream>

namespace Ms {

//---------------------------------------------------------
//   PluginManager
//---------------------------------------------------------

PluginManager::PluginManager(QWidget* parent)
   : QDialog(parent)
      {
      setObjectName("PluginManager");
      setupUi(this);
      setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
      //connect(definePluginShortcut, SIGNAL(clicked()), SLOT(definePluginShortcutClicked()));
      //connect(clearPluginShortcut, SIGNAL(clicked()), SLOT(clearPluginShortcutClicked()));
      //connect(reloadPlugins, SIGNAL(clicked()), SLOT(reloadPluginsClicked()));
      readSettings();
      createListWidget();
      this->comboBox->addItem("OFF");
      this->comboBox->addItem("ON");

      all->addItem(new QListWidgetItem("ABC_import"));
      all->addItem(new QListWidgetItem("Colornotes"));
      all->addItem(new QListWidgetItem("Createscore"));
      all->addItem(new QListWidgetItem("Notenames"));
      all->addItem(new QListWidgetItem("Random"));
      all->addItem(new QListWidgetItem("Random2"));
      all->addItem(new QListWidgetItem("run"));
      all->addItem(new QListWidgetItem("Scorelist"));
      all->addItem(new QListWidgetItem("Walk"));
      /**QString searchText = this->SearchText->text();
      int listWidgetSize = this->allPlugins->count();

      for (int k1 = 0; k1 < listWidgetSize; k1++)
          {
          if (this->allPlugins->item(k1)->text().startsWith(searchText))
          {
               this->allPlugins->item(k1)->setHidden(false);
          }
          else
          {
               this->allPlugins->item(k1)->setHidden(true);
          }
      }**/
      }

//---------------------------------------------------------
//   init
//---------------------------------------------------------

void PluginManager::init()
      {
      //
      // initialize local shortcut table
      //    we need a deep copy to be able to rewind all
      //    changes on "Abort"
      //
      qDeleteAll(localShortcuts);
      localShortcuts.clear();
      foreach(const Shortcut* s, Shortcut::shortcuts())
            localShortcuts[s->key()] = new Shortcut(*s);
      shortcutsChanged = false;
      loadList(false);
      connect(pluginList, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(pluginLoadToggled(QListWidgetItem*)));
      connect(allPlugins, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(pluginLoadToggled(QListWidgetItem*)));
      //connect(systemPlugins, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(pluginLoadToggled(QListWidgetItem*)));
      connect(pluginList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
         SLOT(pluginListItemChanged(QListWidgetItem*, QListWidgetItem*)));
      }

//---------------------------------------------------------
//   loadList - populate the listbox.
//---------------------------------------------------------

void PluginManager::loadList(bool forceRefresh)
      {
      QStringList saveLoaded; // If forcing a refresh, the load flags are lost. Keep a copy and reapply.
      int n = preferences.pluginList.size();
      if (forceRefresh && n > 0) {
            for (int i = 0; i < n; i++) {
                  PluginDescription& d = preferences.pluginList[i];
                  if (d.load) {
                        saveLoaded.append(d.path);
                        mscore->unregisterPlugin(&d);  // This will force the menu to rebuild.
                        }
                  }
            }
      preferences.updatePluginList(forceRefresh);
      n = preferences.pluginList.size();
      pluginList->clear();
      allPlugins->clear();
      for (int i = 0; i < n; ++i) {
            PluginDescription& d = preferences.pluginList[i];
            Shortcut* s = &d.shortcut;
            localShortcuts[s->key()] = new Shortcut(*s);
            if (saveLoaded.contains(d.path)) d.load = true;
            QListWidgetItem* item = new QListWidgetItem(QFileInfo(d.path).completeBaseName(),  pluginList);
            item->setFlags(item->flags() | Qt::ItemIsEnabled);
            item->setCheckState(d.load ? Qt::Checked : Qt::Unchecked);
            item->setData(Qt::UserRole, i);
            QListWidgetItem* item2 = new QListWidgetItem(QFileInfo(d.path).completeBaseName(),  allPlugins);
            item2->setFlags(item2->flags() | Qt::ItemIsEnabled);
       }




      //systemPlugins->addItem(new QListWidgetItem("item2"));
      //systemPlugins->addItem(new QListWidgetItem("item3"));
      prefs = preferences;
      if (n) {
            pluginList->setCurrentRow(0);
            pluginListItemChanged(pluginList->item(0), 0);
            }
      }
/**void PluginManager::on_installedPlugins_itemChanged(QListWidgetItem* changed)
{
    bool checked = changed->checkState() == Qt::Checked;
    int index = 0;
    for (; pluginList->item(index) != changed; index++) ;
    QMessageBox::information(NULL,"Clicked", "You " + QString(checked ? "checked" : "unchecked") + " item " + QString::number(index + 1) + ".");
}**/
//---------------------------------------------------------
//   apply
//---------------------------------------------------------

void PluginManager::accept()
      {
      if (shortcutsChanged) {
            shortcutsChanged = false;
            foreach(const Shortcut* s, localShortcuts) {
                  Shortcut* os = Shortcut::getShortcut(s->key());
                  if (os) {
                        if (!os->compareKeys(*s))
                              os->setKeys(s->keys());
                        }
                  }
            Shortcut::dirty = true;
            }
      int n = prefs.pluginList.size();
      for (int i = 0; i < n; ++i) {
            PluginDescription& d = prefs.pluginList[i];
            if (d.load)
                  mscore->registerPlugin(&d);
            else
                  mscore->unregisterPlugin(&d);
            }
      preferences = prefs;
      preferences.write();
      disconnect(pluginList, SIGNAL(itemChanged(QListWidgetItem*)));
      disconnect(pluginList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)));
      QDialog::accept();
      }

//---------------------------------------------------------
//   closeEvent
//---------------------------------------------------------

void PluginManager::closeEvent(QCloseEvent* ev)
      {
      emit closed(false);
      QWidget::closeEvent(ev);
      }

//---------------------------------------------------------
//   pluginListItemChanged
//---------------------------------------------------------

void PluginManager::pluginListItemChanged(QListWidgetItem* item, QListWidgetItem*)
      {
      if (!item)
            return;
      int idx = item->data(Qt::UserRole).toInt();
      const PluginDescription& d = prefs.pluginList[idx];
      QFileInfo fi(d.path);
      pluginName->setText(fi.completeBaseName());
      pluginPath->setText(fi.absolutePath());
      pluginVersion->setText(d.version);
      //pluginShortcut->setText(d.shortcut.keysToString());
      pluginDescription->setText(d.description);


      }
void PluginManager::installed(QListWidgetItem *item){
    if(item->checkState() == Qt::Checked)
        item->setBackgroundColor(QColor("#ffffb2"));
}
//---------------------------------------------------------
//   pluginLoadToggled
//---------------------------------------------------------

void PluginManager::createListWidget(){
    //systemPlugins = new QListWidget;
    QStringList strList;
    strList << "monitor" << "mouse" << "keyboard" << "hard disk drive"
            << "graphic card" << "sound card" << "memory" << "motherboard";

    //systemPlugins->addItems(strList);
}

void PluginManager::pluginLoadToggled(QListWidgetItem* item)
      {
      int idx = item->data(Qt::UserRole).toInt();
      PluginDescription* d = &prefs.pluginList[idx];
      d->load = (item->checkState() == Qt::Checked);
      /**for (; pluginList->item(index) != changed; index++) ;

      //allPlugins.append(item);**/
      prefs.dirty = true;
      }

//---------------------------------------------------------
//   definePluginShortcutClicked
//---------------------------------------------------------

void PluginManager::definePluginShortcutClicked()
      {
      QListWidgetItem* item = pluginList->currentItem();
      if (!item)
            return;
      int idx = item->data(Qt::UserRole).toInt();
      PluginDescription* pd = &prefs.pluginList[idx];
      Shortcut* s = &pd->shortcut;
      ShortcutCaptureDialog sc(s, localShortcuts, this);
      int rv = sc.exec();
      if (rv == 0)            // abort
            return;
      if (rv == 2)            // replace
            s->clear();

      s->addShortcut(sc.getKey());
      QAction* action = s->action();
      action->setShortcuts(s->keys());
      mscore->addAction(action);

      //pluginShortcut->setText(s->keysToString());
      prefs.dirty = true;
      }

//---------------------------------------------------------
//   reloadPluginShortcutClicked
//---------------------------------------------------------

void PluginManager::reloadPluginsClicked()
      {
      loadList(true);
      QMessageBox::information(0,
            tr("MuseScore"),
            tr("Plugins reloaded."),
            QMessageBox::Ok, QMessageBox::NoButton);
      }

//---------------------------------------------------------
//   clearPluginShortcutClicked
//---------------------------------------------------------

void PluginManager::clearPluginShortcutClicked()
      {
      QListWidgetItem* item = pluginList->currentItem();
      if (!item)
            return;
      int idx = item->data(Qt::UserRole).toInt();
      PluginDescription* pd = &prefs.pluginList[idx];
      Shortcut* s = &pd->shortcut;
      s->clear();

      QAction* action = s->action();
      action->setShortcuts(s->keys());
//    mscore->addAction(action);

      //pluginShortcut->setText(s->keysToString());
      prefs.dirty = true;
      }

//---------------------------------------------------------
//   writeSettings
//---------------------------------------------------------

void PluginManager::writeSettings()
      {
      MuseScore::saveGeometry(this);
      }

//---------------------------------------------------------
//   readSettings
//---------------------------------------------------------

void PluginManager::readSettings()
      {
      MuseScore::restoreGeometry(this);
      }

}

void Ms::PluginManager::on_pushButton_2_clicked()
{
    QString link = "https://musescoreonlinepluginstore.000webhostapp.com/src/Login.php";
    QDesktopServices::openUrl(QUrl(link));
}


void Ms::PluginManager::on_comboBox_activated(const QString)
{
    if (comboBox->currentText()== "ON"){
        qDebug() << "ON";
        int n = prefs.pluginList.size();
        for (int i = 0; i < n; ++i) {
              PluginDescription& d = prefs.pluginList[i];
              if (d.load)
                    mscore->registerPlugin(&d);
              else
                    mscore->unregisterPlugin(&d);
              }
    }
}

