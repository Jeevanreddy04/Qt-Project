import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

ApplicationWindow {
    id: root
    width: 1000
    height: 700
    visible: true
    title: "Camera Player"

    property int frameNonce: 0

    Material.theme: Material.Light

    Dialog {
        id: errorDialog
        title: "Error"
        modal: true
        standardButtons: Dialog.Ok

        property string message: ""

        contentItem: Text {
            text: errorDialog.message
            wrapMode: Text.WordWrap
        }
    }

    Connections {
        target: cameraController

        function onFrameUpdated() {
            frameNonce++
            video.source = "image://camera/frame?" + frameNonce
        }

        function onErrorOccurred(message) {
            errorDialog.message = message
            errorDialog.open()
        }
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 12

            Label {
                text: "Camera Player"
                font.pixelSize: 18
                Layout.alignment: Qt.AlignVCenter
            }

            Item { Layout.fillWidth: true }

            Label {
                text: cameraController.playing
                      ? (cameraController.paused ? "Paused" : "Playing")
                      : "Stopped"
                Layout.alignment: Qt.AlignVCenter
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Image {
                id: video
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                cache: false
                source: "image://camera/frame?0"
            }

            Label {
                anchors.centerIn: parent
                text: "Press Play to start the camera"
                visible: !cameraController.playing
                opacity: 0.6
            }
        }

        Frame {
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent
                spacing: 8

                Button {
                    text: "Play"
                    onClicked: cameraController.play()
                }

                Button {
                    text: "Pause"
                    enabled: cameraController.playing && !cameraController.paused
                    onClicked: cameraController.pause()
                }

                Button {
                    text: "Resume"
                    enabled: cameraController.playing && cameraController.paused
                    onClicked: cameraController.resume()
                }

                ToolSeparator {}

                Button {
                    text: "Rewind"
                    enabled: cameraController.playing && cameraController.paused
                    onClicked: cameraController.rewind()
                }

                Button {
                    text: "Forward"
                    enabled: cameraController.playing && cameraController.paused
                    onClicked: cameraController.forward()
                }

                Item { Layout.fillWidth: true }

                Label {
                    text: "Resolution: " + cameraController.resolutionText
                    opacity: 0.7
                }
            }
        }

        GroupBox {
            title: "Camera Settings"
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent
                spacing: 12

                Label { text: "Resolution" }
                ComboBox {
                    model: cameraController.availableResolutions
                    onActivated: cameraController.setResolutionString(currentText)

                    Component.onCompleted: {
                        const idx = model.indexOf(cameraController.resolutionText)
                        if (idx >= 0)
                            currentIndex = idx
                    }
                }

                Label { text: "Step Frames" }
                SpinBox {
                    from: 1
                    to: 300
                    value: cameraController.stepFrames
                    onValueModified: cameraController.stepFrames = value
                }

                Label { text: "Buffer (sec)" }
                SpinBox {
                    from: 1
                    to: 30
                    value: cameraController.bufferSeconds
                    onValueModified: cameraController.bufferSeconds = value
                }

                Item { Layout.fillWidth: true }
            }
        }
    }
}
