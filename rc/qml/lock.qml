import QtQuick 2.3
 
Rectangle {
	id: root

	gradient: Gradient {
		GradientStop { position: 0.0; color: "#222222" }
		GradientStop { position: 1.0; color: "#000000" }
	}

	Image {
		id: rect
		width: 64; height: 64
		smooth: true
		source: "50-screenlock.png"
		x: root.width / 2 - rect.width / 2
		y: root.height / 2 - rect.height / 2
		
		MouseArea {
			id: ma
			anchors.fill: parent
			drag.target: rect
			drag.axis: Drag.XandYAxis
			onReleased: {
				var x = rect.x + rect.width / 2 - root.width / 2
				if(x < 0) x = -x
				var y = rect.y + rect.height / 2 - root.height / 2
				if(y < 0) y = -y
				if(x > 50 || y > 50) {
					console.log("Dragged significantly")
					rootController.dismissWidget()
				} else {
					rect.x = root.width / 2 - rect.width / 2
					rect.y = root.height / 2 - rect.height / 2
				}
			}
		}
	}
}
