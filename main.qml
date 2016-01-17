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

    //------------Plane selection stuff ahead ------------------------------------------------------------
    property variant activePlane : null //Plane that is currently opened in the Details Dialog
    property bool    activePlaneIsBound : false //Whether or not the activePlane is at least bound to one button, and therefore the map cannot be moved
    property variant boundPlanes : ({}) //An object which maps a button number 0..3 to a plane object, to save the button->plane mapping

    //Binds the currently active plane to a button, or unbinds the button if no plane is active
    function bindCurrentPlane(button) {
        boundPlanes[button] = activePlane;
        if(activePlane) {
            mapOfEurope.center = activePlane.CurrentCoordinate; //center plane in screen
            activePlaneIsBound = true;
            ioc.setState(button,true); //Turn on led for the corresponding button
        }
    }

    //Opens the dialog for the passed plane, or closes the dialog if no valid plane is passed
    function openPlane(plane) {
        if(!plane) {
            closePlane();
            return;
        }
        activePlaneIsBound = false;
        activePlane=plane;
        for(var i=0; i<ioc.buttonCount(); i++) {
            //Turn led of button either on or off, depending on whether the button is bound to the current Plane
            if(boundPlanes[i] === plane) {
                ioc.setState(i,true);
                activePlaneIsBound = true;
                mapOfEurope.center =plane.CurrentCoordinate;
            } else {
                ioc.setState(i,false);
            }
        }
        detailsDialog.open(plane);
    }

    //Closes the details dialog
    function closePlane() {
        detailsDialog.close();
        activePlaneIsBound = false;
        activePlane=null;
        //Turn all leds off
        for(var i=0; i<ioc.buttonCount(); i++) {
           ioc.setState(i,false);
        }
    }

    //Changes the zoomlevel of the map to a level depending on the poti value
    function changeZoomLevel(potiVoltage) {
        //poti value is a real from 0 ... 1.8 (Volt)
       mapOfEurope.zoomLevel = 4+potiVoltage*6; //minimum zoomlevel will be 4, maximum will be around 14.8
       if(activePlane!=null) {
           mapOfEurope.center = activePlane.CurrentCoordinate;
       }
    }

    //-----------------Window content ahead -------------------

    property variant topLeftEurope: QtPositioning.coordinate(51, -11)
    property variant bottomRightEurope: QtPositioning.coordinate(43,19)
    property variant centerOfEurope: QtPositioning.coordinate(46.8,8.1);
    property variant viewOfEurope: QtPositioning.rectangle(topLeftEurope, bottomRightEurope)
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
         gesture.activeGestures: activePlaneIsBound ? 1 : 7

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
                        onClicked: openPlane(display)
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
        onClosed: closePlane()
    }

    SettingsDialog {
        id: settingsDialog
        providers: planeModel.providers
        supportedMapTypes: mapOfEurope.supportedMapTypes
    }

    IoController {
        id: ioc
        onButtonLongPressed: bindCurrentPlane(button)
        onButtonPressed: openPlane(boundPlanes[button])
        onPotiValueChanged: changeZoomLevel(newValue)
    }
}
