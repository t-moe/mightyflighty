import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtPositioning 5.5
import QtLocation 5.5
import QtQuick.Layouts 1.2
import MightyFlighty 1.0

ApplicationWindow {
    id: appwindow
    visible: true
    title: "MightyFlighty"


    property variant topLeftEurope: QtPositioning.coordinate(51, -11)
    property variant bottomRightEurope: QtPositioning.coordinate(43,19)
    property variant centerOfEurope: QtPositioning.coordinate(46.8,8.1);
    property variant viewOfEurope: QtPositioning.rectangle(topLeftEurope, bottomRightEurope)
    property variant activePlane : null
    //property variant planeModel  <-- planeModel is a property of the (root)context and will be set from outside, before the component is created

    Map {
         id: mapOfEurope
         anchors.centerIn: parent
         anchors.fill: parent
         visibleRegion: viewOfEurope
         activeMapType : settingsDialog.activeMapType
         plugin: Plugin {
            name: "osm" //open street map backend
         }

         /*Plane {
            pilotName:"anton"
            coordinate: centerOfEurope
         }*/


         MapPolyline {
            id: mapRoute
            visible: detailsDialog.routeVisible
            path: detailsDialog.routeData
            line.color: "blue"
            line.width: 2
         }

         MapItemView {
            model: planeModel
            delegate: Component {
                Plane {
                    coordinate: display.CurrentCoordinate
                    pilotName: display.CallSign
                    bearing: display.Heading
                    opacity: (activePlane==null || activePlane===display) ? 1 : 0.2

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                          detailsDialog.open(display);
                          activePlane=display;
                        }
                    }
                }
            }
         }
    }


    Rectangle {
        id: exitScreen
        anchors.fill: parent
        anchors.centerIn: parent
        z:100
        visible: false
        color: "black"
        function activate() {
            visible= true;
            timer.start();
        }

        Timer {
            id:timer
            interval: 300
            repeat: false
            onTriggered: {
                appwindow.close();
            }
        }

        Text{
            text: "Bye"
            color: "white"
            font.pixelSize: 48
            anchors.centerIn: parent
        }
    }

    Button {
        id: exitButton
        anchors.margins: 5
        anchors.right: parent.right
        anchors.top: parent.top
        width: 50
        height: 50
        Image {
                anchors.fill: parent
                source: "exit.png"
            }
        onClicked: exitScreen.activate()
    }

    Button {
        id: settingsButton
        anchors.margins: 5
        anchors.right: exitButton.left
        anchors.top: parent.top
        width: 50
        height: 50
        Image {
                anchors.fill: parent
                source: "settings.png"
            }
        onClicked: settingsDialog.open()

    }

    Button {
        anchors.margins: 5
        anchors.right: settingsButton.left
        anchors.top: parent.top
        width: 50
        height: 50
        Image {
                anchors.fill: parent
                source: "center.png"
            }
        onClicked: mapOfEurope.center = centerOfEurope
    }

    DetailsDialog {
        id: detailsDialog
        onClosed:  activePlane = null

    }

    SettingsDialog {
        id: settingsDialog
        providers: planeModel.providers
        supportedMapTypes: mapOfEurope.supportedMapTypes
    }

    IoController {
        onButtonPressed: {
            setState(IoController.Led1,button===IoController.Button1);
        }
        Component.onCompleted: {
            setState(IoController.Led3,true);
        }
    }

}
