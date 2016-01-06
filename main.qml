import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtPositioning 5.5
import QtLocation 5.5

ApplicationWindow {
    id: appwindow
    visible: true
    title: qsTr("MightyFlighty")


    property variant topLeftEurope: QtPositioning.coordinate(51, -11)
    property variant bottomRightEurope: QtPositioning.coordinate(43,19)
    property variant centerOfEurope: QtPositioning.coordinate(46.8,8.1);
    property variant viewOfEurope: QtPositioning.rectangle(topLeftEurope, bottomRightEurope)
    property int i: 0;


    Map {
         id: mapOfEurope
         anchors.centerIn: parent
         anchors.fill: parent
         visibleRegion: viewOfEurope
         plugin: Plugin {
            name: "osm" //open street map backend
         }

         /*Plane {
            pilotName:"anton"
            coordinate: centerOfEurope
         }*/

         MapItemView {
            model: planeModel
            delegate: Component {

                Plane {
                    coordinate: display.CurrentCoordinate
                    pilotName: display.CallSign
                    bearing: display.Heading
                }
            }
         }
    }


    Button {
        id: settingsButton
        anchors.margins: 5
        anchors.right: parent.right
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
            mapOfEurope.center = centerOfEurope;
        }
    }

    Dialog {
        id: settingsDialog
        title: "Settings"
        width: 600
        height: 400
        contentItem: Rectangle {
            Button {
                anchors.margins: 5
                anchors.right: parent.right
                anchors.top: parent.top
                text: "X"
                onClicked: {
                    if(settingsRow2.visible === true) {
                        settingsRow2.visible = false;
                        settingsRow2.content.parent = null
                        settingsRow.visible = true;
                    } else {
                        settingsDialog.close()
                    }
                }
            }

            Text {
                id: settingsLabel
                anchors.margins: 5
                anchors.left: parent.left
                anchors.top: parent.top
                font.pixelSize: 24
                text: "Settings"
            }

            Row {
                id: settingsRow
                anchors.top: settingsLabel.bottom
                anchors.left: parent.left
                anchors.margins: 5
                spacing: 5

                GroupBox {
                    title: "Map Style"
                    height: 240
                    width: 200

                    //Combobox is not supported when using eglfs :( . See: https://bugreports.qt.io/browse/QTBUG-38405
                    ListView {
                        id: mapStyleList
                        anchors.fill: parent
                        model: mapOfEurope.supportedMapTypes
                        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
                        highlightFollowsCurrentItem: true
                        interactive: false
                        delegate: Item { //Template for how to render a single row
                            width: parent.width
                            height: 18
                            Text {
                                text: name //use the name property as text
                                //font.pixelSize: 20
                             }
                            MouseArea {
                                 anchors.fill: parent
                                 onClicked: mapStyleList.currentIndex = index
                            }
                          }
                       /*   //only nececessary if initial value is not 0
                            currentIndex: (function() {
                            var ts = mapStyleList.model;
                            for(var i=0;i<ts.length; i++) {
                                if(ts[i] === mapOfEurope.activeMapType) return i;
                            }
                        }())*/

                        onCurrentItemChanged:{
                            //convert index to item (QDeclarativeGeoMapType)
                            mapOfEurope.activeMapType = mapStyleList.model[mapStyleList.currentIndex];
                        }
                    }
                }

                GroupBox {
                    title: "Data Providers"
                    height: 240
                    width: 200
                    ListView {
                        model: planeModel.providers //providers is a QObjectList containing AbstractProviders
                        interactive: false
                        anchors.fill: parent
                        delegate: Item { //Template for how to render a single row
                            width: parent.width
                            Row {
                                spacing: 5.0
                                CheckBox {
                                    text: model.modelData.name
                                    anchors.verticalCenter: parent.verticalCenter
                                    onClicked: model.modelData.enabled = checked //modify model if checkbox state changes
                                    Component.onCompleted: checked = model.modelData.enabled //load checked state on startup
                                }
                                Button {
                                    anchors.margins: 10
                                    width: 20
                                    height: 20
                                    Image {
                                            anchors.fill: parent
                                            source: "settings.png"
                                        }
                                    onClicked: {
                                        settingsRow2.content = model.modelData.configItem;
                                        settingsRow2.content.parent= settingsRow2;
                                        settingsRow.visible =false;
                                        settingsRow2.visible = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Row { //Container for the "settings widget" of the providers
                id: settingsRow2
                property variant content
                visible: false
                anchors.top: settingsLabel.bottom
                anchors.left: parent.left
                anchors.margins: 5
                spacing: 5
            }

        }
    }


}
