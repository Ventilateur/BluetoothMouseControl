package serialCommunication;

import java.awt.AWTException;
import java.awt.Robot;
import java.awt.Toolkit;

import processing.core.PApplet;
import processing.serial.Serial;

public class MouseControl extends PApplet {

	private static final float _OFFSET_ASCII = 32.0f;
	private static final float _MAX_POSITIF_VAL = 155.0f;
	private static final float _MAX_8_BITS_VAL = 255.0f;
	private static final int _MODE = 0;
	private static final int _dR_VAL = 1;
	private static final int _dP_VAL = 2;
	private static final int _FRAME_LENGTH = 5;
	private static final char _RESET_MODE = 'R';
	private static final char _MOVE_MODE = 'M';
	private static final char _STOP_MODE = 'S';
	
	private Serial serialPort;
	private String incoming;
	private Robot mouse;
	private static int screenWidth;
	private static int screenHeight;
	private int x, xPrev, y, yPrev;
	private float dR, dP, aX, aY, bX, bY, cX, cY;
	private boolean move;

	public MouseControl() {
		screenWidth = (int) Toolkit.getDefaultToolkit().getScreenSize().getWidth();
		screenHeight = (int) Toolkit.getDefaultToolkit().getScreenSize().getHeight();
		x = xPrev = screenWidth;
		y = yPrev = screenHeight;
		dR = dP = 0.0f;
		aX = aY = 1.0f;
		bX = bY = 0.7f;
		cX = cY = 0.04f;
		move = false;
		try {
			mouse = new Robot();
		} catch (AWTException e) {
			println("Robot not supported!!!");
			exit();
		}
	}

	public void openComm(String portName, int baudRate) {
		try {
			serialPort = new Serial(this, portName, baudRate);
			serialPort.bufferUntil('\n');
			System.out.println("Port " + portName + " is open, baud rate is set to " + baudRate);
		} catch (RuntimeException e) {
			System.out.println("ERROR: Cannot open port");
			e.printStackTrace();
			System.exit(0);
		}
	}
	
	public void setCoefficientsAxeX(float aX, float bX, float cX) {
		this.aX = aX; this.bX = bX; this.cX = cX;
	}
	
	public void setCoefficientsAxeY(float aY, float bY, float cY) {
		this.aY = aY; this.bY = bY; this.cY = cY;
	}
	
	private void calculateMove() {
		// decode from ASCII-based values to readable values 
        if (dR < _MAX_POSITIF_VAL) dR -= _OFFSET_ASCII;
        else dR -= _MAX_8_BITS_VAL; 
        if (dP < _MAX_POSITIF_VAL) dP -= _OFFSET_ASCII;
        else dP -= _MAX_8_BITS_VAL;
      
        // calculate absolute position
        if (dR != 0.0f) x = xPrev - (int)((aX + bX * exp((float)(cX * abs(dR)))) * dR / abs(dR));
        if (dP != 0.0f) y = yPrev + (int)((aY + bY * exp((float)(cY * abs(dP)))) * dP / abs(dP));
      
        // avoid out-of-window movement
        if (x < 0) x = 0;
        else if (x > screenWidth) x = screenWidth;
        if (y < 0) y = 0;
        else if (y > screenHeight) y = screenHeight;
	}
	
	private void processData(String data) {
		if (data.length() == _FRAME_LENGTH) {
			// reset position if mode = R
		    if (data.charAt(_MODE) == _RESET_MODE) {
				move = false;
				x = xPrev = screenWidth/2; 
				y = yPrev = screenHeight/2;
		    // move if mode = C
		    } else if (data.charAt(_MODE) == _MOVE_MODE) {
		        move = true;
		        dR = (float)(data.charAt(_dR_VAL));
		        dP = (float)(data.charAt(_dP_VAL));
		        calculateMove();
		    // stop if mode = S
		    } else if (data.charAt(_MODE) == _STOP_MODE) {
		        move = false; 
		    }
		}
	}

	public void serialEvent(Serial serialPort) {
		incoming = serialPort.readString();
		processData(incoming);
		if (move) {
			mouse.mouseMove(x, y);
			xPrev = x; 
			yPrev = y;
		}
	}

}
