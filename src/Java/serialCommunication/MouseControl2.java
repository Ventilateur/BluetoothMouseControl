package serialCommunication;

import java.awt.AWTException;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.event.InputEvent;

import processing.core.PApplet;
import processing.serial.Serial;

public class MouseControl2 extends PApplet {

	private static final int _MODE = 0;
	private static final int _LB = 1;
	private static final int _RB = 2;
	private static final int _dX_VAL = 3;
	private static final int _dY_VAL = 4;
	private static final int _FRAME_LENGTH = 6;
	private static final char _RESET_MODE = 'R';
	private static final char _MOVE_MODE = 'M';
	private static final char _STOP_MODE = 'S';
	private static final char _BUTTON_PRESSED = 'P';
	private static final char _BUTTON_RELEASED = 'R';
	private static final byte _END_OF_FRAME = 127;
	
	private Serial serialPort;
	private byte[] incoming;
	private Robot mouse;
	private static int screenWidth;
	private static int screenHeight;
	private int x, xPrev, y, yPrev;
	private float dR, dP, aX, aY, bX, bY, cX, cY;
	private boolean move;
	private boolean LBpressed, RBpressed, LBisPressed, RBisPressed;

	public MouseControl2() {
		screenWidth = (int) Toolkit.getDefaultToolkit().getScreenSize().getWidth();
		screenHeight = (int) Toolkit.getDefaultToolkit().getScreenSize().getHeight();
		x = xPrev = screenWidth;
		y = yPrev = screenHeight;
		dR = dP = 0.0f;
		aX = aY = 1.0f;
		bX = bY = 0.7f;
		cX = cY = 0.04f;
		move = LBpressed = RBpressed = LBisPressed = RBisPressed = false;
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
			serialPort.bufferUntil(_END_OF_FRAME);
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
        // calculate absolute position
        if (dR != 0.0f) x = xPrev - (int)((aX + bX * exp((float)(cX * abs(dR)))) * dR / abs(dR));
        if (dP != 0.0f) y = yPrev - (int)((aY + bY * exp((float)(cY * abs(dP)))) * dP / abs(dP));
      
        // avoid out-of-window movement
        if (x < 0) x = 0;
        else if (x > screenWidth) x = screenWidth;
        if (y < 0) y = 0;
        else if (y > screenHeight) y = screenHeight;
	}
	
	private void processData(byte[] data) {
		if (data.length == _FRAME_LENGTH) {
		    // check mouse mode
		    switch (data[_MODE]) {
		    case (_MOVE_MODE):
		    	move = true;
	        	dR = (float)(data[_dX_VAL]);
	        	dP = (float)(data[_dY_VAL]);
	        	calculateMove();
	        	break;
		    case (_RESET_MODE):
		    	move = false;
				x = xPrev = screenWidth/2; 
				y = yPrev = screenHeight/2;
				break;
		    case (_STOP_MODE):
		    	move = false;
		    	break;
		    default:
		    	move = false;
		    	break;
		    }
		    
		    // check LB's state
		    switch (data[_LB]) {
		    case (_BUTTON_PRESSED):
		    	LBpressed = true;
		    	break;
		    case (_BUTTON_RELEASED):
		    	LBpressed = false;
		    	break;
		    default:
		    	LBpressed = false;
		    	break;
		    }
		    
		    // check RB's state
		    switch (data[_RB]) {
		    case (_BUTTON_PRESSED):
		    	RBpressed = true;
		    	break;
		    case (_BUTTON_RELEASED):
		    	RBpressed = false;
		    	break;
		    default:
		    	RBpressed = false;
		    	break;
		    }
		}
	}

	public void serialEvent(Serial serialPort) {
		incoming = serialPort.readBytes();
		for (byte b : incoming) System.out.println(b);
		processData(incoming);
		if (move) {
			mouse.mouseMove(x, y);
			xPrev = x; 
			yPrev = y;
		}
		if (LBpressed && !LBisPressed) {
			mouse.mousePress(InputEvent.BUTTON1_MASK);
			LBisPressed = true;
		} else if (!LBpressed && LBisPressed) {
			mouse.mouseRelease(InputEvent.BUTTON1_MASK);
			LBisPressed = false;
		}
		if (RBpressed && !RBisPressed) {
			mouse.mousePress(InputEvent.BUTTON3_MASK);
			RBisPressed = true;
		} else if (!RBpressed && RBisPressed) {
			mouse.mouseRelease(InputEvent.BUTTON3_MASK);
			RBisPressed = false;
		}
	}

}
