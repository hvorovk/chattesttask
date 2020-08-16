import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Controls.Material.impl 2.14

import "../Singletons"

Popup {
    id: control

    property int autoHideInterval: 5000

    function show(message) {
        autoHideTimer.start();
        messageLabel.text = message;

        control.open();
    }


    padding: TTStyle.padding / 2

    Material.elevation: 1

    background: Rectangle {
        radius: 4
        color: TTStyle.darkPrimaryColor

        layer.enabled: control.Material.elevation > 0
        layer.effect: ElevationEffect {
            elevation: control.Material.elevation
        }

        MouseArea {
            anchors.fill: parent
            anchors.margins: -1

            onClicked: {
                autoHideTimer.stop();
                control.close();
            }
        }
    }

    contentItem: Label {
        id: messageLabel

        font.pixelSize: 18
        color: "white"
        anchors.verticalCenter: parent.verticalCenter
    }

    Timer {
        id: autoHideTimer

        repeat: false
        interval: autoHideInterval

        onTriggered: control.close()
    }
}
