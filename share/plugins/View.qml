import QtQuick 2.8
import MuseScore 3.0

MuseScore {
      version: "3.1"
      description: "Demo plugin to demonstrate the use of a ScoreView"
      menuPath: "Plugins.ScoreView"
      pluginType: "dialog"

      width:  400
      height: 400
      Component.onCompleted: {
            if (typeof curScore === 'undefined')
                  Qt.quit();

            scoreview.setScore(curScore);
            }

      ScoreView {
            id: scoreview
            anchors.fill: parent
            color: "white"
            MouseArea {
                  anchors.fill: parent
                  onClicked: Qt.quit()
                  }
            }
      }
