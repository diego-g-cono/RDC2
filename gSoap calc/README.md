Ejercicio de calculo con SOAP:
---------------------------------------------------------------
Para compilar el servidor hay que poner lo siguiente en Ubuntu:
gcc soapServer.c soapC.c calcs.c -lgsoap -lm -o bcs
Y para ejecutgar el servidor:
./bcs
---------------------------------------------------------------
Para compilar el servidor hay que poner lo siguiente en Ubuntu:
gcc soapClient.c soapC.c calcc.c -lgsoap -o bcc
Y para ejecutgar el servidor:
./bcc

