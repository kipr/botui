#!/bin/sh
echo "Generating Manager class..."
qdbusxml2cpp -c Manager -p Manager manager.xml
echo "Generating Agent class..."
qdbusxml2cpp -a Agent -c Agent -p Agent agent.xml
echo "Generating Service class..."
qdbusxml2cpp -c Service -p Service service.xml
echo "Generating Technology class..."
qdbusxml2cpp -c Technology -p Technology technology.xml

echo "Moving Headers..."
mv *.h ../include
echo "Moving Sources..."
mv *.cpp ../src
echo "Done!"