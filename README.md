# esp8266OneWireTemp
esp8266 json response with onewire ds18b20  (arduino impl)

Read multiple ds18b20 sensors with one esp8266.
Responces is given in json where the given adresses are from the ds18b20's.
Temp given in celcius.

{"info":
  {"devices":3,"parasite_power":"0","macaddress":"18:FE:34:E6:20:C5","sensors":
    [
      {"address":"288E0BA50500008A","temp":"-22.37"},
      {"address":"28FE69A505000080","temp":"10.75"},
      {"address":"283D42A5050000CE","temp":"10.63"}
    ]
  }
}
