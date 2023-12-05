import QtQuick
import com.acr.aoivus 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
    EllipseWidgetQml {
        id: id_ell
        width: 200
        height: 200
        x: 100
        y: 100
    }
}
