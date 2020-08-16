import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14

import "../Singletons"
import "../SimpleControls"

Rectangle {
    id: control

    property bool isRegister: false
    property bool inProgress: false

    function register() {
        inProgress = true;
        comunicationManager.registration(loginTF.text, paswordTF.text)
    }

    function login() {
        inProgress = true;
        comunicationManager.login(loginTF.text, paswordTF.text)
    }

    onInProgressChanged: {
        if (inProgress) {
            inProgressPopup.open();
        } else {
            inProgressPopup.close();
        }
    }

    color: TTStyle.appBackground

    Connections {
        target: comunicationManager

        onRegistrationError: {
            inProgress = false;
            toast.show("Registration error")
            // TODO: process errors
        }

        onAuthError: {
            inProgress = false;
            toast.show("Authentication error")
            // TODO: process errors
        }

        onRegistrationSuccess: {
            inProgress = false;
            toast.show("Registration successfully finished.")
            isRegister = false;
            paswordTF.clear();
            paswordTF.forceActiveFocus();
        }

        onAuthSuccess: {
            inProgress = false;
            loginTF.text = comunicationManager.loginString;
            paswordTF.clear();
        }

        onDisconnected: {
            inProgress = false;
            loginTF.text = comunicationManager.loginString;
            paswordTF.clear();
        }
    }

    MouseArea {
        // For prevent clicks on undelaying elements
        anchors.fill: parent
    }

    TTPane {
        anchors.centerIn: parent
        width: 300

        Material.background: TTStyle.primaryColor

        Column {
            spacing: 20
            width: parent.width

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: isRegister ? qsTr("Registration") : qsTr("Hello!")
                font.pixelSize: 24
                color: TTStyle.labelTextColorLight
            }

            Label {
                visible: !isRegister
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Please log in to the system")
                font.pixelSize: 18
                color: TTStyle.labelTextColorLight
            }

            TTTextField {
                id: loginTF

                placeholderText: isRegister ? qsTr("New login") : qsTr("Login")
                maximumLength: 20
                width: parent.width
            }

            TTTextField {
                id: paswordTF

                placeholderText: isRegister ? qsTr("New password") : qsTr("Password")
                echoMode: TTTextField.PasswordEchoOnEdit
                maximumLength: 20
                width: parent.width
            }

            TTButton {
                visible: !isRegister
                text: qsTr("Log in")
                width: parent.width

                onClicked:  {
                    login();
                }
            }

            TTButton {
                text: qsTr("Register")
                width: parent.width

                onClicked:  {
                    if (!isRegister) {
                        return isRegister = true;
                    }
                    register();
                }
            }

            TTButton {
                visible: isRegister
                text: qsTr("Back to authorization")
                width: parent.width
                onClicked:  {
                    if (isRegister) {
                        return isRegister = false;
                    }
                }
            }
        }
    }

    Popup {
        id: inProgressPopup

        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        modal: true
        width: 150
        height: 150
        closePolicy: Popup.NoAutoClose

        background: Item {
            BusyIndicator {
                anchors.fill: parent
                Material.accent: TTStyle.accentColor
                running: true
            }
        }
    }
}
