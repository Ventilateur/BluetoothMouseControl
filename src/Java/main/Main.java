package main;

import processing.serial.Serial;

import java.util.ArrayList;
import java.util.List;
import java.util.Arrays;
import java.util.Scanner;

import serialCommunication.*;

public class Main {
	
	public static MouseControl serialPort = new MouseControl();
	public static String portName;
	public static int baudRate;
	public static Scanner keyboard = new Scanner(System.in);
	public static List<Integer> baudRates = new ArrayList<Integer>();
	
	public static void printPortsInfo() {
		System.out.println("Here come all available ports:");
		for (String port : Serial.list()) {
			System.out.println(port);
		}
	}
	
	public static void choosePort() {
		printPortsInfo();
		System.out.println("Enter your port here:");
		portName = keyboard.nextLine();
		while (!Arrays.asList(Serial.list()).contains(portName)) {
			System.out.println("Please re-enter a valid port name:");
			portName = keyboard.nextLine();
		}
	}
	
	public static boolean checkBaudRate(int baudRate) {
		return (baudRate == 300  || baudRate == 1200  || baudRate == 2400  || baudRate == 4800 ||
				baudRate == 9600 || baudRate == 19200 || baudRate == 38400 || baudRate == 57600);
	}
	
	public static void chooseBaudRate() {
		System.out.println("Set baud rate:");
		baudRate = Integer.parseInt(keyboard.nextLine());
		while (!checkBaudRate(baudRate)) {
			System.out.println("Set valid baud rate:");
			baudRate = Integer.parseInt(keyboard.nextLine());
		}
	}
	
	public static void main(String[] args) {
		switch (args.length) {
		case (0):
			choosePort();
			chooseBaudRate();
			break;
		case (1):
			System.out.println("Port " + args[0] + " chosen");
			chooseBaudRate();
			break;
		case (2):
			System.out.println("Port " + args[0] + " chosen");
			baudRate = Integer.parseInt(args[1]);
			if (!checkBaudRate(baudRate)) {
				baudRate = 9600;
				System.out.println("Invalid baud rate, baud rate is set to default value of 9600");
			}
			break;
		default:
			System.out.println("Too many arguments, maximum 2 expected!");
			System.exit(0);
			break;
		}
		serialPort.openComm(portName, baudRate);
		while (true);
	}
	
}
