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
  * Button FUNC, LB, RB are connected to Teensy's pins 20, 6 and 2. They are configured to run on mode INPUT_PULLDOWN, so the other buttons' pins go to 3.3V. 
  </br>

**4. Run:**
  Compile and load MouseControl.ino to Teensy board, remember to configure the bluetooth's baud rate to 57600bps </br>
  Run the .jar file with one of these commands: </br>
  * *java -jar MouseControl.jar*
  * *java -jar MouseControl.jar COMx*
  * *java -jar MouseControl.jar COMx 57600*
  </br>

**5. Technical details:**
  * MPU-9150 is used here for capturing movements. This project uses only gyroscope's measures so you can use any IMU that you like, however some small modification will be needed. 
  * Next step is to compute relative angles between two measures and send these datas via bluetooth. In order to minimize the frame's size, data will be transfered in 6 bytes: [MODE][LR][RB][depX][depY][\n] and will be decoded by character.
    * [MODE] Can be changed by the button's states :
      * 'R' : reset mouse position to the screen's center - button clicked (<200ms)
      * 'M' : move the mouse - button not pushed
      * 'S' : stop moving mouse - button pushed
    * [LB] Left button's state, can either be pressed ('P') or released ('R')
    * [RB] Right button's state, can either be pressed ('P') or released ('R')
    * [depX] Gyro's Z measure (no, not X)
    * [depY] Gyro's Y measure
    * [\n] Delimit the end of frame
  * These datas will be read by a java program and will be processed in order to have a smooth mouse movement. Here I used an exponential function to transfrom angles into mouse positions.



