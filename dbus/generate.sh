#!/bin/sh

rm -Rf include
rm -Rf src

generate()
{
	qdbusxml2cpp -c ${1} -p ${1} -N -i NetworkManagerConnection.h ${2}.xml
}

echo "Generating NetworkManager class..."
generate NMNetworkManager org.freedesktop.NetworkManager.xml

echo "Generating Device class..."
generate NMDevice org.freedesktop.NetworkManager.Device.xml  

echo "Generating DeviceEthernet class..."
generate NMDeviceEthernet org.freedesktop.NetworkManager.Device.Wired.xml 

echo "Generating DeviceWifi class..."
generate NMDeviceWifi org.freedesktop.NetworkManager.Device.WifiP2P.xml

echo "Generating Settings class..."
generate NMSettings org.freedesktop.NetworkManager.Settings.xml

echo "Generating SettingsConnection class..."
generate NMSettingsConnection org.freedesktop.NetworkManager.Settings.Connection.xml

echo "Generating SecretAgent class..."
generate NMSecretAgent org.freedesktop.NetworkManager.SecretAgent.xml

echo "Generating AccessPoint class..."
generate NMAccessPoint org.freedesktop.NetworkManager.AccessPoint.xml

echo "Generating Serial class..."
generate Serial org.kipr.Serial

mkdir -p src
mkdir -p include

echo "Moving Headers..."
mv *.h include

echo "Moving Sources..."
mv *.cpp src

echo "Done!"
