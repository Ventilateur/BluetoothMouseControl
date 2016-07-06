# BluetoothMouseControl
**Control computer mouse "on air" via bluetooth, technical details can be found at the end of file** </br>

1. Hardware required: </br>
  * Teensy 3.2
  * MPU9150 breakout board
  * HC-06 bluetooth module
  * Push button
  </br>

2. Software required: </br>
  * Java SE on computer
  * All required libraries are in folder *lib*
  </br>

3. Wiring: </br>
  * MPU-9150 ========== Teensy        
    * VCC -------------------> 3.3V   
    * GND -------------------> GND
    * SCL -------------------> 19 (SCL0)
    * SDA -------------------> 18 (SDA0)
  * HC-06
4. Run: </br>
  Compile and load MouseControl.ino to Teensy board, remember to configure the bluetooth's baud rate to 57600bps </br>
  Run the .jar file with one of these commands: </br>
  * *java -jar MouseControl.jar*
  * *java -jar MouseControl.jar COMx*
  * *java -jar MouseControl.jar COMx 57600*
  </br>




