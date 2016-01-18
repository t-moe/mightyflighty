import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtPositioning 5.5
import QtLocation 5.5
import QtQuick.Layouts 1.2

Dialog {
    id: settingsDialog
    title: "Settings"
    width: 600
    height: 400

    property variant providers;
    property variant supportedMapTypes;
    property variant activeMapType;

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
            font.pixelSize: 32
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
                height: 260
                width: 210
                
                //Combobox is not supported when using eglfs :( . See: https://bugreports.qt.io/browse/QTBUG-38405
                ListView {
                    id: mapStyleList
                    anchors.fill: parent
                    model: supportedMapTypes
                    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
                    highlightFollowsCurrentItem: true
                    interactive: false
                    delegate: Item { //Template for how to render a single row
                        width: parent.width
                        height: 32
                        Text {
                            text: name //use the name property as text
                            font.pixelSize: 24
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: mapStyleList.currentIndex = index
                        }
                    }
                    /*   //only nececessary if initial value is not 0
                            currentIndex: (function() {
                            for(var i=0;i<supportedMapTypes.length; i++) {
                                if(supportedMapTypes[i] === activeMapType) return i;
                            }
                        }())*/
                    
                    onCurrentItemChanged:{
                        //convert index to item (QDeclarativeGeoMapType)
                        activeMapType = supportedMapTypes[mapStyleList.currentIndex];
                    }
                }
            }
            
            GroupBox {
                title: "Data Providers"
                height: 260
                width: 350
                ListView {
                    model: providers //providers is a QObjectList containing AbstractProviders
                    interactive: false
                    anchors.fill: parent
                    delegate: Item { //Template for how to render a single row
                        width: parent.width
                        height: 25
                        Row {
                            spacing: 5.0
                            CheckBox {
                                id: cb
                                transform: Scale {
                                    origin.x: 0;
                                    origin.y: height/2;
                                    xScale: 1.3;
                                    yScale: 1.3;
                                }
                                anchors.verticalCenter: parent.verticalCenter
                                onClicked: model.modelData.enabled = checked //modify model if checkbox state changes
                                Component.onCompleted: checked = model.modelData.enabled //load checked state on startup
                            }
                            Text {
                                text:  model.modelData.name
                                font.pixelSize: 24
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            Button {
                                visible: model.modelData.configItem!==null
                                anchors.margins: 10
                                anchors.verticalCenter: parent.verticalCenter
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
