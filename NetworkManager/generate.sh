#!/bin/sh

rm -Rf include
rm -Rf src

echo "Generating NetworkManager class..."
qdbusxml2cpp -c NMNetworkManager -p NMNetworkManager nm-manager.xml

echo "Generating Device class..."
qdbusxml2cpp -c NMDevice -p NMDevice nm-device.xml

echo "Generating DeviceEthernet class..."
qdbusxml2cpp -c NMDeviceEthernet -p NMDeviceEthernet nm-device-wifi.xml

echo "Generating DeviceWifi class..."
qdbusxml2cpp -c NMDeviceWifi -p NMDeviceWifi nm-device-wifi.xml

echo "Generating Settings class..."
qdbusxml2cpp -c NMSettings -p NMSettings nm-settings.xml

echo "Generating SettingsConnection class..."
qdbusxml2cpp -c NMSettingsConnection -p NMSettingsConnection nm-settings-connection.xml

echo "Generating SecretAgent class..."
qdbusxml2cpp -c NMSecretAgent -p NMSecretAgent nm-secret-agent.xml

mkdir -p src
mkdir -p include

echo "Moving Headers..."
mv *.h include

echo "Moving Sources..."
mv *.cpp src

echo "Done!"