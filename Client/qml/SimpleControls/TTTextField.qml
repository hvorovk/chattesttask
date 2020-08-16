import QtQuick 2.14
import QtQuick.Controls 2.14

import "../Singletons"

TextField {
    id: control

    property bool error: false
    property string iconSource: ""
    property size iconSize: Qt.size(0, 0)

    topPadding: 0
    bottomPadding: 0
    leftPadding: iconSource == "" ? TTStyle.padding * 0.5 : iconSize.width + TTStyle.padding * 0.75
    rightPadding: leftPadding
    selectByMouse: true
    placeholderTextColor: TTStyle.darkPlaceHolderColor
    selectedTextColor: TTStyle.appBackground
    selectionColor: TTStyle.accentColor
    font.pixelSize: 18
    color: TTStyle.labelTextColor

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 48
        color: TTStyle.chatBackground
        radius: 4

        border {
            width: 1
            color: error ? TTStyle.errorColor : control.activeFocus ? TTStyle.accentColor : TTStyle.darkPlaceHolderColor
        }
    }
}
