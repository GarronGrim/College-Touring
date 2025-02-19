import QtQuick 6.6
import QtQuick.Controls 6.6

ApplicationWindow {
    visible: true
    width: 1400
    height: 800
    title: "College Touring"
    color: "#212121"

    property real spacing: width / 20
    property real fontScale: Math.min(width, height) * 0.025

    Rectangle {
        anchors.fill: parent
        color: "#212121"
    }
    Rectangle {
        id: contentArea
        anchors.fill: parent
        anchors.margins: spacing
        color: "#212121"


        ComboBox {
            id: referenceCollegeDropdown
            anchors.top: parent.top
            anchors.left: parent.left
            width: parent.width / 6
            model: collegeModel
            textRole: "name"

            onCurrentIndexChanged: {
                console.log("Current index:", currentIndex);
                console.log("Current text:", currentText);
            }

            background: Rectangle {
                color: "#333333"
                radius: 5
                border.color: "#555555"
                border.width: 1
            }

            contentItem: Text {
                text: referenceCollegeDropdown.currentIndex >= 0 ? referenceCollegeDropdown.currentText : "Select College"
                color: "#E0E0E0"
                font.pixelSize: fontScale
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.fill: parent
            }

            delegate: ItemDelegate {
                width: referenceCollegeDropdown.width
                height: 40

                contentItem: Text {
                    text: model.name
                    color: "#E0E0E0"
                    font.pixelSize: fontScale
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    anchors.fill: parent
                }

                background: Rectangle {
                    width: referenceCollegeDropdown.width
                    height: parent.height
                    color: highlighted ? "#444444" : "#222222"
                    border.color: highlighted ? "#666666" : "transparent"
                }
            }

            popup: Popup {
                id: dropdownPopup
                y: referenceCollegeDropdown.height
                width: referenceCollegeDropdown.width
                implicitHeight: Math.min(200, referenceCollegeDropdown.count * 40)
                padding: 0

                background: Rectangle {
                    width: referenceCollegeDropdown.width
                    height: dropdownPopup.implicitHeight
                    color: "#222222"
                    border.color: "#444444"
                    radius: 5
                }

                contentItem: ListView {
                    width: referenceCollegeDropdown.width
                    height: dropdownPopup.implicitHeight
                    model: referenceCollegeDropdown.model

                    delegate: ItemDelegate {
                        width: referenceCollegeDropdown.width
                        height: 40

                        contentItem: Text {
                            text: model.name
                            color: "#E0E0E0"
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            anchors.fill: parent
                        }

                        background: Rectangle {
                            width: referenceCollegeDropdown.width
                            height: parent.height
                            color: highlighted ? "#444444" : "#222222"
                            border.color: highlighted ? "#666666" : "transparent"
                        }

                        onClicked: {
                            console.log("Clicked index:", index, "Name:", model.name);
                            referenceCollegeDropdown.currentIndex = index;
                            collegeModel.setReferenceCollege(model.name);
                            dropdownPopup.close();
                        }
                    }
                }
            }
        }

        ComboBox {
            id: modeSelectionDropdown
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width / 6
            model: ["Default", "Custom"]

            background: Rectangle {
                color: "#333333"
                radius: 5
                border.color: "#555555"
                border.width: 1
            }

            contentItem: Text {
                text: modeSelectionDropdown.displayText
                color: "#E0E0E0"
                font.pixelSize: fontScale
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.fill: parent
            }

            delegate: ItemDelegate {
                width: modeSelectionDropdown.width
                height: 40

                contentItem: Text {
                    text: modelData
                    color: "#E0E0E0"
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    anchors.fill: parent
                }

                background: Rectangle {
                    width: modeSelectionDropdown.width
                    height: parent.height
                    color: highlighted ? "#444444" : "#222222"
                    border.color: highlighted ? "#666666" : "transparent"
                }
            }
        }

        Rectangle {
            id: planTrip
            anchors.top: parent.top
            anchors.right: parent.right
            width: parent.width / 6
            height: 40
            color: "#333333"
            radius: 5
            border.color: "#555555"
            border.width: 1

            Text {
                text: "Plan Trip!"
                color: "#E0E0E0"
                font.pixelSize: fontScale
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.fill: parent
            }
        }

        Flickable {
            id: flickable1
            anchors.top: referenceCollegeDropdown.bottom
            anchors.topMargin: spacing
            anchors.left: parent.left
            width: (parent.width - 2 * spacing) / 3
            height: parent.height - referenceCollegeDropdown.height - 2 * spacing
            contentWidth: width
            contentHeight: collegeList.height

            ListView {
                id: collegeList
                width: parent.width
                height: contentHeight
                model: collegeModel
                delegate: Rectangle {
                    width: parent.width
                    height: 50
                    color: "#333333"
                    border.color: "#555555"
                    radius: 5

                    Text {
                        text: model.name + " - Distance: " + model.distance
                        color: "#E0E0E0"
                        font.pixelSize: fontScale
                        anchors.centerIn: parent
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            souvenirModel.clear()
                            var souvenirs = collegeModel.getSouvenirs(model.name)
                            for (var i = 0; i < souvenirs.length; i++) {
                                souvenirModel.append(souvenirs[i])
                            }
                        }
                    }
                }
            }
        }

        Flickable {
            id: flickable2
            anchors.top: referenceCollegeDropdown.bottom
            anchors.topMargin: spacing
            anchors.left: flickable1.right
            anchors.leftMargin: spacing
            width: (parent.width - 2 * spacing) / 3
            height: parent.height - referenceCollegeDropdown.height - 2 * spacing
            contentWidth: width
            contentHeight: souvenirList.height

            ListView {
                id: souvenirList
                width: parent.width
                height: contentHeight
                model: ListModel { id: souvenirModel }
                delegate: Rectangle {
                    width: parent.width
                    height: 50
                    color: "#333333"
                    border.color: "#555555"
                    radius: 5

                    Text {
                        text: model.name + " - $" + model.price
                        color: "#E0E0E0"
                        font.pixelSize: fontScale
                        anchors.centerIn: parent
                    }
                }
            }
        }

        Rectangle {
            id: summaryTable
            anchors.top: referenceCollegeDropdown.bottom
            anchors.topMargin: spacing
            anchors.left: flickable2.right
            anchors.leftMargin: spacing
            height: parent.height - referenceCollegeDropdown.height - 2 * spacing
            width: (parent.width - 2 * spacing) / 3
            color: "#1E1E1E"
            border.color: "#444444"
            radius: 5

            Column {
                anchors.fill: parent
                spacing: 2

                Rectangle {
                    width: parent.width
                    height: parent.height / 12
                    color: "#333333"

                    Row {
                        anchors.fill: parent
                        spacing: 2

                        Text {
                            text: "Name"
                            color: "#E0E0E0"
                            font.pixelSize: fontScale
                            width: parent.width / 3
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text {
                            text: "# of Souvenirs"
                            color: "#E0E0E0"
                            font.pixelSize: fontScale
                            width: parent.width / 3
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text {
                            text: "$ Spent"
                            color: "#E0E0E0"
                            font.pixelSize: fontScale
                            width: parent.width / 3
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }

                Repeater {
                    model: 10
                    delegate: Rectangle {
                        width: summaryTable.width
                        height: parent.height / 12
                        color: index % 2 === 0 ? "#222222" : "#2E2E2E"
                        border.color: "#444444"

                        Row {
                            anchors.fill: parent
                            spacing: 2

                            Text {
                                text: ""
                                color: "#E0E0E0"
                                font.pixelSize: fontScale
                                width: parent.width / 3
                                horizontalAlignment: Text.AlignHCenter
                            }
                            Text {
                                text: ""
                                color: "#E0E0E0"
                                font.pixelSize: fontScale
                                width: parent.width / 3
                                horizontalAlignment: Text.AlignHCenter
                            }
                            Text {
                                text: ""
                                color: "#E0E0E0"
                                font.pixelSize: 14
                                width: parent.width / 3
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: parent.height / 12
                    color: "#333333"

                    Row {
                        anchors.fill: parent
                        spacing: 2

                        Text {
                            text: "Total:"
                            color: "#E0E0E0"
                            font.pixelSize: fontScale
                            width: parent.width / 3
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text {
                            text: ""
                            color: "#E0E0E0"
                                            font.pixelSize: fontScale
                            width: parent.width / 3
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text {
                            text: ""
                            color: "#E0E0E0"
                            font.pixelSize: fontScale
                            width: parent.width / 3
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
        }

        Rectangle {
            id: nextDestination
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            width: parent.width / 6
            height: referenceCollegeDropdown.height
            color: "#333333"
            radius: 5
            border.color: "#555555"
            border.width: 1

            Text {
                text: "Next Destination"
                color: "#E0E0E0"
                font.pixelSize: fontScale
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.fill: parent
            }
        }


        Rectangle {
            id: totalDistance
            anchors.left: nextDestination.right
            anchors.bottom: parent.bottom
            width: parent.width / 6
            height: referenceCollegeDropdown.height
            color: "#333333"
            radius: 5
            border.color: "#555555"
            border.width: 1

            Text {
                text: "Total Distance: 0"
                color: "#E0E0E0"
                font.pixelSize: fontScale
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.fill: parent
            }
        }
    }
}
