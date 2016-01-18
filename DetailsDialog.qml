import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtPositioning 5.5
import QtLocation 5.5
import QtQuick.Layouts 1.2

Rectangle {
    color: "#EEEEEE"
    border.color: "black"
    id: detailsDialog
    width: 400
    height: 160
    anchors.left: parent.left
    anchors.bottom: parent.bottom
    anchors.margins: 5
    visible: false
    signal closed();

    function open(model) {
        detailsRepeater.model = model;
        visible = true;
    }
    function close() {
        if(visible) {
            visible = false;
            closed();
        }
    }
    
    Repeater {
        id: detailsRepeater
        
        Item {
            anchors.fill: parent
            Button {
                anchors.margins: 10
                anchors.right: parent.right
                anchors.top: parent.top
                text: "X"
                onClicked: close()
            }
            
            GridLayout {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 10
                columns: 2
                rowSpacing: 2
                Text {
                    Layout.columnSpan: 2
                    font.pixelSize: 24
                    text: "Details "+model.CallSign
                }
                
                Text {
                    text: "CallSign:"
                    font.pixelSize: 14
                }
                Text {
                    text: model.CallSign
                    font.pixelSize: 14
                }
                Text {
                    text: "Speed:"
                    font.pixelSize: 14
                }
                Text {
                    text: model.Speed +" km/h"
                    font.pixelSize: 14
                }
                Text {
                    text: "Altitude:"
                    font.pixelSize: 14
                }
                Text {
                    text: model.Altitude +" m"
                    font.pixelSize: 14
                }
                
                Text {
                    text: "Departure Airport:"
                    font.pixelSize: 14
                }
                Text {
                    text: model.AdditionalData.departureAirport || "Unknown"
                    font.pixelSize: 14
                }
                
                Text {
                    text: "Arrival Airport:"
                    font.pixelSize: 14
                }
                Text {
                    text: model.AdditionalData.arrivalAirport || "Unknown"
                    font.pixelSize: 14
                }
            }
        }   
    }
}
