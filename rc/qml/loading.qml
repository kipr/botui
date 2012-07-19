import QtQuick 1.0
import ZapBsComponents 1.0
 
Rectangle {
	id: root
	width: parent.width
	height: parent.height

	gradient: Gradient {
		GradientStop { position: 0.0; color: "#555555" }
		GradientStop { position: 1.0; color: "#000000" }
	}

	BusyIndicator {
		id: loading
		width: 50
		height: 50
		anchors.centerIn: parent
		foregroundColor: "#FFFFFF"
		backgroundColor: "#000000"
		
		RotationAnimation {
			target: loading
			property: "rotation" // Suppress a warning
			from: 0
			to: 360
			direction: RotationAnimation.Clockwise
			duration: 1000
			loops: Animation.Infinite
			running: true
		}
	}
}