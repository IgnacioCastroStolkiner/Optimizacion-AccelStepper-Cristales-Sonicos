# Optimizacion-AccelStepper-Cristales-Sonicos
Proyecto para optimizar la librería AccelStepper para que pueda alcanzar mayor velocidad.

AccelStepper-master es la librería

Cristal_Steppers.ino es el código de las arduino.

Estes proyecto forma parte de mi tesis para la licenciatura de Música y Tecnología en la Universidad de Quilmes.

Necesito controlar 5 motores paso a paso con driver desde una Arduino Mega. Actualmente me encuentro con la 
limitación de que a más de 200 micropasos/s la Arduino deja de responder de forma lineal y poco después deja
de crecer. El dispositivo en que uso estos motores tiene mucha reducción y necesita alcanzar velocidades mayores.

Este proyecto busca investigar la forma de conseguir modificar la librería AccelStepper o el programa actual para
conseguir velocidades mayores dentro de las limitaciones de procesamiento de la arduino. La reducciòn entre el eje del motor y la base circular de las columnas es muy grande y el procesador de la Arduino no llega a tirar la cantidad de pulsos necesarios para que las 5 columnas se muevan rápido a la vez. 

Invito a cualquier persona que tenga ganas de mirar el código o la librería a hacer su aporte y les agradezco a todos!

Prueba 1: Probamos mover motores sueltos bastante mayor velocidad, así que deducimos que probablemente no sea una limitación del driver
ni de torque de los motores.
