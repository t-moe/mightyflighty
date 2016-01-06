import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

Item {
    Repeater {
        //The repeater is necessary because the model will not be ready when, the component is loaded
        //With the repeater, the template below will be instantiated as soon as we set the model property of the repeater (from code)
        GridLayout {
            columns: 2
            Text {
                text: "App Key:"
            }
            TextField {
                id: appKeyText
                text: model.appKey
                Layout.fillWidth: true
            }

            Binding { //Emulate Two-Way Binding by adding the reverse binding manually
                //Another way would be to subscribe to textChanged and update the model from there
                //model.appKey will be updated if the value of appKeyText.text changes
                target: model
                property: "appKey"
                value: appKeyText.text
            }


            Text {
                text: "App Id:"
            }
            TextField {
                id: appIdText
                text: model.appId
                Layout.fillWidth: true
            }
            Binding {
                target: model
                property: "appId"
                value: appIdText.text
            }

            Text {
                Layout.columnSpan :2
                text: "You can get free credentinals for evaluation purposes from flightstats.com"
            }
        }
    }
}
