# BluetoothMouseControl
Control computer mouse "on air" via bluetooth </br>

Hardware required: 
* Teensy 3.2
* MPU9150 breakout board
* HC-06 bluetooth module
* Push button
</br>

Software required: </br>
* Java SE on computer
* All required libraries are in folder *lib*
</br>

Compile and load MouseControl.ino to Teensy board, remember to configure the bluetooth's baud rate to 57600bps </br>
Run the .jar file with one of these commands: </br>
* *java -jar MouseControl.jar*
* *java -jar MouseControl.jar COMx*
* *java -jar MouseControl.jar COMx 57600*
</br>




