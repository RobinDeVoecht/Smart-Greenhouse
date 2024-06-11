# Smart-Greenhouse
Kweekbakje geautomatiseerd met behulp van sensoren en actuatoren.
### Benodigde hardware voor project Smart greenhouse 


-	Raspberry Pi (of een vergelijkbare microcontroller)
-	ESP32
-	Temperatuur- en vochtigheidssensor (BME280 + DS18B20)
-	Grondvochtigheidssensor
-	Lichtsensor (LDR)
-	Waterlevel sensor
-	Waterpomp
-	ventilator
-	LED-strip (beter nog is groeilampen)
-	Relaismodule (om de waterpomp, ventilator en lampen te schakelen)
-	5V voeding
-	Prototyping board
-	Muntplantje (of andere plant)
-	LCD scherm


### Benodigde software
-	Arduino IDE
-	InfluxDB
-	Mosquitto (MQTT broker)
-	Paho-MQTT (Python MQTT client)
-	Putty



### Beschrijving van systeem
-	Sensoren: 
Deze meten temperatuur, luchtvochtigheid, grondvochtigheid en lichtintensiteit.

-	Actuatoren:
De waterpomp, ventilator en ledstrip worden bestuurd op basis van sensorwaarden.

-	Data opslag: 
Sensorgegevens worden opgeslagen in een InfluxDB database.

-	MQTT: 
Gebruik MQTT voor communicatie tussen sensoren/actuatoren en de Raspberry Pi.




### TIPS / EXTRA's
-	Zorg ervoor dat je een goed schema hebt voor het solderen van de hardware componenten om verwarring te vermijden

-	Zorg voor extra verbindingsdraden (die makkelijk te solderen zijn)

-	Boormachine met bijbehorende vijzen

-	Zorg dat je een gode stevige behuizing hebt waarmee je kan werken. 


