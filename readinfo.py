from bs4 import BeautifulSoup
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

fp = open("webpage/index.html", "r+")
soup = BeautifulSoup(fp, "lxml")
soup.find_all("td")
soup.find(id="DaTi")
tag = soup.td
tag.string.replace_with(result["Time"])
tag
fp.close()

#fp = open("webpage/index.html", "r+")
#soup = BeautifulSoup(fp, "lxml")
#soup.find_all("td")
#soup.find(id="tempC")
#tag = soup.td
#tag.string.replace_with(str)(result["temperatureCelcius"])
#tag
#fp.close()

#fp = open("webpage/index.html", "r+")
#soup = BeautifulSoup(fp, "lxml")
#soup.find_all("td")
#soup.find(id="tempF")
#tag = soup.td
#tag.string.replace_with(str(result["temperatureFahrenheit"])
#tag
#fp.close()

#fp = open("webpage/index.html", "r+")
#soup = BeautifulSoup(fp, "lxml")
#soup.find_all("td")
#soup.find(id="intAle")
#tag = soup.td
#tag.string.replace_with(str(result["intruderAlert"]))
#tag
#fp.close() 

html = soup.prettify("utf-8")
with open("webpage/index.html", "wb") as file:
	file.write(html)
