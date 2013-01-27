#!/bin/sh

rm -Rf include
rm -Rf src

generate()
{
	qdbusxml2cpp -c ${1} -p ${1} -N -i NetworkManagerConnection.h ${2}.xml
}

echo "Generating NetworkManager class..."
generate NMNetworkManager nm-manager

echo "Generating Device class..."
generate NMDevice nm-device

echo "Generating DeviceEthernet class..."
generate NMDeviceEthernet nm-device-ethernet

echo "Generating DeviceWifi class..."
generate NMDeviceWifi nm-device-wifi

echo "Generating Settings class..."
generate NMSettings nm-settings

echo "Generating SettingsConnection class..."
generate NMSettingsConnection nm-settings-connection

echo "Generating SecretAgent class..."
generate NMSecretAgent nm-secret-agent

echo "Generating AccessPoint class..."
generate NMAccessPoint nm-access-point

echo "Generating Serial class..."
generate Serial org.kipr.Serial

mkdir -p src
mkdir -p include

echo "Moving Headers..."
mv *.h include

echo "Moving Sources..."
mv *.cpp src

echo "Done!"