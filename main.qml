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
            activePlaneIsBound = true;
            ioc.setState(button,true); //Turn on led for the corresponding button

            //fix plane in map center, and subscribe to update events
            mapOfEurope.center =activePlane.CurrentCoordinate;
            activePlane.coordinateChanged.connect(updateActivePlaneData);
        }
    }

    //Opens the dialog for the passed plane, or closes the dialog if no valid plane is passed
    function openPlane(plane) {
        if(!plane) { //no plane passed
            closePlane(); //close instead
            return;
        }

        //remove existing connections to old active Plane
        if(activePlane) {
            activePlane.additionalDataChanged.disconnect(updateActivePlaneData);
            activePlane.aboutToBeRemoved.disconnect(closePlane);
            if(activePlaneIsBound) {
                activePlane.coordinateChanged.disconnect(updateActivePlaneData);
                activePlaneIsBound = false;
            }
        }

        //Make new plane the "active plane" and subscribe to signals
        activePlane=plane;
        activePlane.additionalDataChanged.connect(updateActivePlaneData); //to update the "route" of the plane
        activePlane.aboutToBeRemoved.connect(closePlane); //close dialog if plane disappears

        for(var i=0; i<ioc.buttonCount(); i++) {
            //Turn led of button either on or off, depending on whether the button is bound to the current Plane
            if(boundPlanes[i] === plane) {
                ioc.setState(i,true);
                if(!activePlaneIsBound) { //only if plane is not already bound
                    //fix plane in map center, and subscribe to update events
                    mapOfEurope.center =plane.CurrentCoordinate;
                    activePlane.coordinateChanged.connect(updateActivePlaneData);
                    activePlaneIsBound = true;
                }
            } else {
                ioc.setState(i,false);
            }
        }

        //Check if route data available and display it
        if(plane.AdditionalData && plane.AdditionalData.route) {
            mapRoute.path = plane.AdditionalData.route;
            mapRoute.visible = true;
        } else {
            mapRoute.visible = false;
        }

        detailsDialog.open(plane); //open dialog
    }

    //Closes the details dialog
    function closePlane() {
        detailsDialog.close();

        //unsubscribe from all events, clear all vars
        if(activePlane) {
            activePlane.additionalDataChanged.disconnect(updateActivePlaneData);
            activePlane.aboutToBeRemoved.disconnect(closePlane);
            if(activePlaneIsBound) {
                activePlane.coordinateChanged.disconnect(updateActivePlaneData);
                activePlaneIsBound = false;
            }
            activePlane=null;
        }

        mapRoute.visible = false;

        //Turn all leds off
        for(var i=0; i<ioc.buttonCount(); i++) {
           ioc.setState(i,false);
        }
    }

    //"Slot" that is called when properties of the current plane have changed and we need to update the map center/route
    function updateActivePlaneData() {
        if(activePlaneIsBound) {
            mapOfEurope.center =activePlane.CurrentCoordinate;
        }
        if(activePlane.AdditionalData && activePlane.AdditionalData.route) {
            mapRoute.path = activePlane.AdditionalData.route;
            mapRoute.visible = true;
        } else {
            mapRoute.visible = false;
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
            visible: false
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
        onClicked: {
            if(!activePlaneIsBound) {
                mapOfEurope.center = centerOfEurope;
            }
        }
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
