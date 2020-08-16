import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.14
import QtQuick.Controls.Material.impl 2.14

import "../Singletons"

Pane {
    id: control

    Material.background: TTStyle.primaryColor
    padding: TTStyle.padding

    background: Rectangle {
        radius: 4
        color: control.Material.background

        layer.enabled: true
        layer.effect: ElevationEffect {
            elevation: 2
        }
    }
}
