#!/bin/sh
echo "Generating Manager class..."
qdbusxml2cpp -c Manager -p Manager manager.xml
echo "Generating Agent class..."
qdbusxml2cpp -c Agent -p Agent agent.xml
echo "Generating Service class..."
qdbusxml2cpp -c Service -p Service service.xml
echo "Generating Technology class..."
qdbusxml2cpp -c Technology -p Technology technology.xml

mkdir -p ../include/connman
mkdir -p ../src/connman

echo "Moving Headers..."
mv *.h ../include/connman
echo "Moving Sources..."
mv *.cpp ../src/connman
echo "Done!"