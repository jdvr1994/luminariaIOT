# luminariaIOT

En este documento se describe todo lo referente a la comuniacion de las Luminarias IOT con los servidores de la Universidad de Nariño.

## Sobre el protocolo

El protocolo de comunicacion usado es MQTT, permitiendole al dispositivo una comuniacion bidireccional, en otras palabras le permite emitir y recibir mensajes desde el servidor haciendo uso de una dinamica basada en suscripciones.

Los dispositivos pueden suscribirse a uno o varios temas, e inclusive a subtemas. Este modo de funcionamiento se describe en la siguiente imagen:

![alt text](https://www.hivemq.com/img/blog/topic_basics.png)

para este caso en particular se tiene una structura multitema, donde el tema principal (superior) es "myhome", el cual contiene al subtema "groundfloor" que ha su vez contiene al subtema "livingroom" ...

Esto permite que la informacion pueda ser emitida a un determinado grupo de oyentes (dispositivos suscritos). Esto sera muy importante a la hora se definir la comunicacion con los dispositivos controladores de las luminarias IOT.

## Sobre los temas (topics)

Para comunicar los controladores de las luminarias con el servidor se propone la siguiente estructura de temas:

### `Controlador a Servidor`
 #### Tema "newData" 
 La informacion sera enviada al servidor como un objeto json con la siguiente estructura (informacion de ejemplo en "data"):
 
  ```js
{ 
   "id":"{{deviceID}}",
   "name" : "{{deviceName}}",
   "mode": "{{modeDriver}}",
   "data":{ 
      "led":{
         "voltage": Number,
         "current": Number
      },
      "pv":{
         "voltage": Number,
         "current": Number
      },
      "battery":{
         "voltage": Number,
         "current": Number
      }
   }
}
```
 
 Cada controlador tiene una ID unica que permitira identificarlo de los demas, de esta forma se podra establecer comunicacion directa con cada dispositivo en particular. De la misma manera cada controlador dispondra de un nombre que podra o no ser unico, de esta manera dos dispositivos con el mismo nombre quedaran suscritos al mismo tema, y se podra generar grupos de luminarias que esten configuradas con el mismo modo de funcionamiento.
 
 Las posibles opciones de modeDriver son:
  "OFF"
  "DEMO"
  "MODE L"
  "MODE T"
  "MODE U"
  "MODE M"
  
  Un objeto de ejemplo se muestra acontinuación:
  
   ```js
{ 
   "id":"e00fce68f6101f77f858ad6f",
   "name" : "biblioteca",
   "mode": "MODE L",
   "data":{ 
      "led":{
         "voltage": 38.5,
         "current": 643.51
      },
      "pv":{
         "voltage": 21.3,
         "current": 413.25
      },
      "battery":{
         "voltage": 13.7,
         "current": 100.38
      }
   }
}

### `Servidor a Controlador`
 #### Tema "changeMode" 
 Todos los controladores de las luminarias estaran suscritos a este tema,esto permitira cambiar el modo de funcionamiento de toda la red de luminarias a la vez.
 
 El dispositivo recibira un objeto json con un elemento "mode" de tipo String que contendra el modo de funcionamiento que se desea establecer en las luminarias.
 
 ```js
{
    "mode": "{{modeDriver}}";
}
```
Las posibles opciones de modeDriver son:
  "OFF"
  "DEMO"
  "MODE L"
  "MODE T"
  "MODE U"
  "MODE M"
  
  Un objeto de ejemplo se muestra a continuacion:
  
```js
{
    "mode": "MODE T";
}
```
  
 #### "changeMode/{{deviceID}}"
 Cada uno de los dispositivos cuenta con una deviceID, esto permitira que cada uno de los dispositivos este suscrito a un tema en particular. La informacion recibida tendra la misma estructura que el tema "changeMode"
 
 #### "changeMode/{{deviceName}}"
 Cada uno de los dispositivos cuenta con un deviceName, esto permitira que cada uno de los dispositivos este suscrito a un tema en particular. La informacion recibida tendra la misma estructura que el tema "changeMode"
 
 
####  Notas
Se aconceja usar la funcion JSON.parse(message) en el lado del servidor para poder recuperar la informacion recibida (enviada por el controlador) en forma de objeto y acceder a cada uno de sus elementos. Ejemplo: 

```js
  JSON.parse(message)
  var bCurrent = message.data.battery.corriente;
```

## License
Copyright 2019-2022 JDVR, Juan David Velasquez Rosero

Released under the LGPL license
