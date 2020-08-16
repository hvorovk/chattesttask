import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.impl 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Controls.Material.impl 2.14

import "../Singletons"

Button {
    id: control

    property int radius: 4
    property color highlightColor: Material.highlightedButtonColor
    property alias contentIconLabel: contentIconLabel
    property alias backgroundRect: backgroundRect


    Material.background: flat ? "transparent" : Qt.darker(TTStyle.accentColor, 1.05)
    Material.foreground: TTStyle.labelTextColor
    Material.elevation: flat ? control.down || control.hovered ? 2 : 0
                                 : control.down ? 1 : 2
    font.pixelSize: 20
    font.capitalization: Font.MixedCase
    font.weight: Font.Medium

    bottomInset: 0
    topInset: 0
    leftInset: 0
    rightInset: 0

    topPadding: 3
    bottomPadding: topPadding
    leftPadding: 10
    rightPadding: leftPadding

    icon.color: !enabled ? TTStyle.setColorAlpha(Material.foreground, 0.5) :
            flat && highlighted ? Material.accentColor :
            highlighted ? Material.primaryHighlightedTextColor : Material.foreground

    contentItem: IconLabel {
        id: contentIconLabel

        spacing: control.spacing
        mirrored: control.mirrored
        display: control.display

        icon: control.icon
        text: control.text
        font: control.font
        color: !control.enabled ? TTStyle.setColorAlpha(control.Material.foreground, 0.5) :
            control.flat && control.highlighted ? control.Material.accentColor :
            control.highlighted ? control.Material.primaryHighlightedTextColor : control.Material.foreground
    }

    background: Rectangle {
        id: backgroundRect

        implicitWidth: 64
        implicitHeight: 48

        radius: control.radius
        color: !control.enabled ? TTStyle.setColorAlpha(control.Material.buttonColor, 0.5) :
                control.highlighted ? control.highlightColor : control.Material.buttonColor

        PaddedRectangle {
            y: parent.height - 4
            width: parent.width
            height: 4
            radius: control.radius
            topPadding: -2
            clip: true
            visible: control.checkable && (!control.highlighted || control.flat)
            color: control.checked && control.enabled ? control.Material.accentColor : control.Material.secondaryTextColor
        }

        layer.enabled: control.enabled && control.Material.buttonColor.a > 0 && !control.flat
        layer.effect: ElevationEffect {
            elevation: control.Material.elevation
        }

        Ripple {
            clipRadius: control.radius
            width: parent.width
            height: parent.height
            pressed: control.pressed
            anchor: control
            active: control.down || control.visualFocus || control.hovered
            color: control.flat && control.highlighted ? control.Material.highlightedRippleColor : "#10000000"
        }
    }
}
