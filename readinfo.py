import urllib.request, json
data = urllib.request.urlopen("http://10.0.1.7/all").read().decode("utf-8")
data = data[data.find("{"):data.find("}")+1]

result = json.loads(data)
print(result)

if(result["intruderAlert"] == "SAFE!"):
	print("You are safe")
else:
	print("INTRUDER!!!")

print("Temperature is: ", result["temperatureCelcius"], "C // " , result["temperatureFahrenheit"], "F")
print("Time is: ", result["Time"]) 
