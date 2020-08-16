pragma Singleton
import QtQuick 2.14

QtObject {
    readonly property color appBackground : "#FFFFFF"
    readonly property color labelTextColor: "#212121"
    readonly property color labelTextColorLight: "#FFFFFF"
    readonly property color accentColor: "#CDDC39"
    readonly property color darkPrimaryColor: "#455A64"
    readonly property color primaryColor: "#607D8B"
    readonly property color chatBackground: "#cfd8dc"
    readonly property color dividerColor: "#BDBDBD"
    readonly property color errorColor: "#F44336"
    readonly property color darkPlaceHolderColor: "#757575"

    readonly property int padding : 20

    // create new color by adjusting alpha channel of existing color
    function setColorAlpha(color, alpha) {
        // Color is transparent then do nothing with it
        if (color.a === 0) {
            return color;
        }

        return Qt.hsla(color.hslHue, color.hslSaturation, color.hslLightness, alpha)
    }
}
