#!/bin/sh

rm -Rf include
rm -Rf src

echo "Generating NetworkManager class..."
qdbusxml2cpp -c NMNetworkManager -p NMNetworkManager -N -i NetworkManagerConnection.h nm-manager.xml

echo "Generating Device class..."
qdbusxml2cpp -c NMDevice -p NMDevice -N -i NetworkManagerConnection.h nm-device.xml

echo "Generating DeviceEthernet class..."
qdbusxml2cpp -c NMDeviceEthernet -p NMDeviceEthernet -N -i NetworkManagerConnection.h nm-device-wifi.xml

echo "Generating DeviceWifi class..."
qdbusxml2cpp -c NMDeviceWifi -p NMDeviceWifi -N -i NetworkManagerConnection.h nm-device-wifi.xml

echo "Generating Settings class..."
qdbusxml2cpp -c NMSettings -p NMSettings -N -i NetworkManagerConnection.h nm-settings.xml

echo "Generating SettingsConnection class..."
qdbusxml2cpp -c NMSettingsConnection -p NMSettingsConnection -N -i NetworkManagerConnection.h nm-settings-connection.xml

echo "Generating SecretAgent class..."
qdbusxml2cpp -c NMSecretAgent -p NMSecretAgent -N -i NetworkManagerConnection.h nm-secret-agent.xml

mkdir -p src
mkdir -p include

echo "Moving Headers..."
mv *.h include

echo "Moving Sources..."
mv *.cpp src

echo "Done!"