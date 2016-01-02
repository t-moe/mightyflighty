import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtPositioning 5.5
import QtLocation 5.5

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("MightyFlighty")


    property variant topLeftEurope: QtPositioning.coordinate(51, -11)
    property variant bottomRightEurope: QtPositioning.coordinate(43,19)
    property variant viewOfEurope: QtPositioning.rectangle(topLeftEurope, bottomRightEurope)


    Map {
         id: mapOfEurope
         anchors.centerIn: parent
         anchors.fill: parent
         visibleRegion: viewOfEurope
         plugin: Plugin {
            name: "osm"
         }
    }

    Button {
        text: "Center on Bern"
        onClicked: {
            console.log("test");
            mapOfEurope.center = QtPositioning.coordinate(46.8,8.1);
        }
    }


}
