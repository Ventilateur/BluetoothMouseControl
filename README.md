# BluetoothMouseControl
**Control computer mouse "on air" via bluetooth, technical details can be found at the end of file** </br>

**1. Hardware required:**
  * Teensy 3.2
  * MPU9150 breakout board
  * HC-06 bluetooth module
  * Push button
  </br>

**2. Software required:**
  * Java SE on computer
  * All required libraries are in folder *lib*
  </br>

**3. Wiring:**
  * MPU-9150 ========== Teensy        
    * VCC -------------------> 3.3V   
    * GND -------------------> GND
    * SCL --------------------> 19 (SCL0)
    * SDA --------------------> 18 (SDA0)
  * HC-06 ============= Teensy
    * VCC -------------------> Vin   
    * GND -------------------> GND
    * Tx ----------------------> 9 (Rx2)
    * Rx ----------------------> 10 (Tx2)
  * Button ==============Teensy
    * Pin0 --------------------> 20
    * Pin1 --------------------> 3.3V
  </br>

**4. Run:**
  Compile and load MouseControl.ino to Teensy board, remember to configure the bluetooth's baud rate to 57600bps </br>
  Run the .jar file with one of these commands: </br>
  * *java -jar MouseControl.jar*
  * *java -jar MouseControl.jar COMx*
  * *java -jar MouseControl.jar COMx 57600*
  </br>

**5. Technical details:**
  * MPU-9150 is used here for capturing movements. A Madgwick filter is then applied in order to fusion the datas from accelerometer
  and gyroscope. The output is rotation angles around axe X (known as Pitch) and axe Y (known as Roll). However, the magnetometer is not used here due to it's speed and complexity. 
  * Next step is to compute relative angles between two measures and send these datas via bluetooth. In order to minimize the frame's size, data will be transfered in 5 bytes: [MODE][ROLL][PITCH][\r][\n] and will be decoded by character.
    * [MODE] Can be changed by the button's states :
      * 'R' : reset mouse position to the screen's center - button clicked (<200ms)
      * 'M' : move the mouse - button not pushed
      * 'S' : stop moving mouse - button pushed
    * [ROLL] Relative roll angle between two measures
    * [PITCH] Relative pitch angle between two measures
    * [\r][\n] Delimit the end of frame, automatically generated by methode println() 
  * These datas will be read by a java program and will be processed in order to have a smooth mouse movement. Here I used an exponential function to transfrom angles into mouse positions.



