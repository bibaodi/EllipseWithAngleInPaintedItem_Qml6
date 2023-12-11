import QtQuick
import com.acr.aoivus 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Using Wheel rotate Ellipse;")
    EllipseWidgetQml {
        id: id_ell
        anchors.fill: parent
    }
}
