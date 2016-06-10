import sys

# Source datafile (one voltage reading per line as float, 10 readings per second)
source = "data"

# Load resistance in ohms
resistance = 5

# Threshold voltage when battery is still usable by desired application
threshold = 0.8

if len(sys.argv) == 2:
    source = sys.argv[1]
    print("Using datafile \"" + source + "\"")
else:
    print("Datafile not provided as firs argument trying \"data\".")

print("Reading input voltages...")
ignored = 0
voltages = []
with open(source) as file:
    for line in file:
        try:
            voltage = float(line)
            #print(voltage)
            voltages.append(voltage)
        except Exception as e:
            ignored = ignored + 1

#print(voltages)
print(str(ignored) + " lines skipped")
print(str(len(voltages)) + " lines processed")



print("Computing capacity...")
usableCapacity = 0
totalCapacity = 0;
usableEnergy = 0;
totalEnergy = 0;

for voltage in voltages:
    current = voltage / resistance
    currentPerHour = current / 3600 / 10
    energyPerHour = currentPerHour * voltage

    totalCapacity = totalCapacity + currentPerHour
    totalEnergy = totalEnergy + energyPerHour

    if voltage > threshold:
        usableCapacity = usableCapacity + currentPerHour
        usableEnergy = usableEnergy + energyPerHour

print("Results:")
print("Total : " + str(totalCapacity * 1000) + " mAh")
print("Total : " + str(totalEnergy) + " Wh")
print("Usable: " + str(usableCapacity * 1000) + " mAh")
print("Usable: " + str(usableEnergy) + " Wh")
        
