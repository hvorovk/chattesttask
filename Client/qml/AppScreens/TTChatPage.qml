import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14

import "../Singletons"
import "../SimpleControls"

Rectangle {
    id: control

    function register() {
        inProgress = true;
    }

    function login() {
        toast.show("Test sentence to test toast working")
        inProgress = true;
    }

    color: TTStyle.appBackground

    MouseArea {
        // For prevent clicks on underlaying elements
        anchors.fill: parent
    }

    ListModel {
        id: messageListModel

        onCountChanged: messageListView.positionViewAtEnd();
    }

    Connections {
        target: comunicationManager

        onSystemMessage: {
            messageListModel.append({
                                        login: "System",
                                        text: message
                                    })
        }
        onGlobalMessageGot: {
            messageListModel.append({
                                        login: sender,
                                        text: message
                                    })
        }

        onAuthSuccess: {
            messageTextArea.clear();
            messageListModel.clear();
        }
    }

    TTPane {
        anchors {
            right: parent.right
            top: parent.top
            left: messageBoxPane.right
            margins: TTStyle.padding
        }

        Column {
            spacing: TTStyle.padding
            width: parent.width

            Label {
                font.pixelSize: 24
                width: parent.width
                text: "@" + comunicationManager.loginString
            }

            TTButton {
                width: parent.width
                text: qsTr("Log out")

                onClicked: logout()
            }
        }
    }

    TTPane {
        anchors {
            left: parent.left
            top: parent.top
            bottom: messageBoxPane.top
            margins: TTStyle.padding
        }

        width: parent.width - 300

        Material.background: TTStyle.primaryColor

        Rectangle {
            anchors.fill: parent

            color: TTStyle.chatBackground
            radius: 4

            ListView {
                id: messageListView

                anchors.fill: parent
                anchors.margins: 4
                spacing: 10
                clip: true
                model: messageListModel

                delegate: Rectangle {
                    property bool myOwn: model.login === comunicationManager.loginString
                    property bool isSystem: model.login === "System"
                    property int maxWidth: control.width - 450 - 2 * TTStyle.padding

                    radius: 4
                    anchors.left: myOwn ? undefined : parent.left
                    anchors.right: myOwn ? parent.right : undefined
                    color: isSystem ? TTStyle.chatBackground : (myOwn ? TTStyle.primaryColor : TTStyle.darkPrimaryColor)

                    width: isSystem ? parent.width : Math.min(contentColumn.implicitWidth, control.width - 450)
                    height: isSystem ? 20 : contentColumn.implicitHeight

                    Label {
                        visible: isSystem
                        anchors.centerIn: parent
                        color: Qt.darker(TTStyle.dividerColor, 1.4)
                        font.pixelSize: 18
                        text: model.text
                    }

                    Column {
                        id: contentColumn

                        width: parent.width

                        visible: !isSystem
                        padding: TTStyle.padding
                        spacing: TTStyle.padding

                        Label {
                            visible: !myOwn
                            font.pixelSize: 20
                            font.weight: Font.Medium
                            color: TTStyle.labelTextColorLight
                            text: model.login
                        }

                        Label {
                            width: Math.min(textMetrics.width, maxWidth)
                            font.pixelSize: 16
                            color: TTStyle.setColorAlpha(TTStyle.labelTextColorLight, 0.8)
                            text: model.text
                            wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                        }

                        TextMetrics {
                            id: textMetrics

                            text: model.text
                            font.pixelSize: 16
                        }
                    }
                }
            }
        }
    }

    TTPane {
        id: messageBoxPane

        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: TTStyle.padding

        width: parent.width - 300
        height: 120

        Material.background: TTStyle.primaryColor

        Rectangle {
            anchors.rightMargin: 100
            anchors.fill: parent
            color: TTStyle.chatBackground
            radius: 4

            ScrollView {
                width: parent.width
                height: parent.height
                contentWidth: parent.width
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                clip: true

                TextArea {
                    id: messageTextArea

                    placeholderText: qsTr("Write a message...")
                    padding: TTStyle.padding
                    topPadding: TTStyle.padding
                    wrapMode: TextEdit.Wrap
                    width: messageBoxPane.width - 40
                    font.pixelSize: 18
                    color: TTStyle.labelTextColor
                    selectByMouse: true
                    placeholderTextColor: TTStyle.darkPlaceHolderColor
                    selectedTextColor: TTStyle.appBackground
                    selectionColor: TTStyle.accentColor

                    background: null
                }
            }
        }

        TTButton {
            text: "⏎"
            width: 80
            height: 80
            anchors.right: parent.right
            font.pixelSize: 48

            onClicked: {
                comunicationManager.sendMessage(messageTextArea.text)
                messageTextArea.clear();
            }
        }
    }
}
