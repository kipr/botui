#!/bin/sh

rm -Rf include
rm -Rf src

generate()
{
	qdbusxml2cpp -c NM${1} -p NM${1} -N -i NetworkManagerConnection.h ${2}.xml
}

echo "Generating NetworkManager class..."
generate NetworkManager nm-manager

echo "Generating Device class..."
generate Device nm-device

echo "Generating DeviceEthernet class..."
generate DeviceEthernet nm-device-ethernet

echo "Generating DeviceWifi class..."
generate DeviceWifi nm-device-wifi

echo "Generating Settings class..."
generate Settings nm-settings

echo "Generating SettingsConnection class..."
generate SettingsConnection nm-settings-connection

echo "Generating SecretAgent class..."
generate SecretAgent nm-secret-agent

echo "Generating AccessPoint class..."
generate AccessPoint nm-access-point

mkdir -p src
mkdir -p include

echo "Moving Headers..."
mv *.h include

echo "Moving Sources..."
mv *.cpp src

echo "Done!"