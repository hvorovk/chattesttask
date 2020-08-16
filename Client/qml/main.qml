import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14

import "Singletons"
import "AppScreens"
import "SpecialControls"

Window {
    id: mainWindow

    property bool authorized: false

    function logout() {
        comunicationManager.logout();
    }

    visible: true
    width: 1100
    height: 760
    title: qsTr("Chat client")

    Material.background: TTStyle.appBackground

    TTAuthPage {
        visible: !authorized
        anchors.fill: parent
    }

    TTChatPage {
        visible: authorized
        anchors.fill: parent
    }

    Rectangle {
        anchors.fill: parent
        color: TTStyle.appBackground

        visible: !comunicationManager.serverConnection

        BusyIndicator {
            anchors.centerIn: parent
            width: 120
            height: 120
        }

        Label {
            color: TTStyle.labelTextColor

            text: qsTr("Connecting to server...")
            font.pixelSize: 32

            anchors.centerIn: parent
            anchors.verticalCenterOffset: -95
        }
    }

    TTToastPopup {
        id: toast

        x: (parent.width - width) / 2
        y: parent.height - height - TTStyle.padding
    }

    Connections {
        target: comunicationManager

        onAuthSuccess: authorized = true;
        onLoggedOut: authorized = false;
        onConnectionError: toast.show(qsTr("Connection error! Trying to reconnect"))

        onDisconnected: {
            authorized = false;
            toast.show(qsTr("Disconnected from server! Trying to reconnect"))
        }
    }

    Component.onCompleted: comunicationManager.connectToServer();
}
