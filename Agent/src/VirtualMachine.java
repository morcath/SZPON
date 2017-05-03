import java.awt.Dimension;
import java.awt.Toolkit;
import java.util.ArrayList;
import java.util.Random;

import javax.swing.*;

public class VirtualMachine {
	JFrame mainFrame;
	JPanel panel = new JPanel();
	JButton exitB;
	JButton overheatB;
	JButton extremeOverheatB;
	
	boolean overheat = true; //zmienna symulujaca nadmierne grzanie systemu - system bedzie sie ogrzewac az do wprowadzenia progu
	boolean extremeOverheat = false; //zmienna symulujaca ekstremalne grzanie systemu - system bedzie sie ogrzewac mimo chlodzenia
	boolean coolingSig = false; //otrzymano sygnal nakazujacy chlodzenie
	boolean heatingSig = false; //otrzymano sygnal nakazujacy grzanie
	boolean measure = true; //otrzymano sygnal nakazujacy pomiar
	boolean alarmState = false;
	
	int screenWidth;
	int screenHeight;
	ArrayList<Integer> temperaturesList = new ArrayList<Integer>();
	VirtualMachine()
	{
		mainFrame = new JFrame();
		mainFrame.setDefaultCloseOperation (JFrame.EXIT_ON_CLOSE);
		mainFrame.setTitle("Maszyna robocza");
		
		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
		screenWidth = (int)screenSize.getWidth();
		screenHeight = (int)screenSize.getHeight();
		
		mainFrame.setBounds((screenWidth/2)-400, (screenHeight/2)-300, 800, 600);
		panel = new JPanel();
		System.out.println("Helo");
		//TODO: dodaæ guziki, ogarn¹æ wykresy itp, chcemy: Label z aktualnym pomiarem, 
		//wykres liniowy (mo¿e?), wykres s³upkowy
		mainFrame.add(panel);
		mainFrame.setVisible(true);
	}
	public void simulate(int minTemperature, int maxTemperature) throws InterruptedException
	{
		temperaturesList.clear();
		Random generator = new Random();
		int startTemperature, temperature = generator.nextInt(6)+18;
		int tempRand;
		startTemperature = temperature;
		temperaturesList.add(startTemperature);
	
		while (measure)
		{
			if (overheat || extremeOverheat)
			{
				++temperature;
			}
			else
			{
				tempRand = generator.nextInt(100);
				if (tempRand > 90)
					++temperature;
				else if (tempRand < 10)
					--temperature;
				else if (tempRand > 45 && tempRand < 55);
				else
				{
					if (1==1)//funkcja uzale¿niaj¹ca wynik losowania od akt temp
						++temperature;
					else
						--temperature;
				}
					
			}
			System.out.println(temperature);
			temperaturesList.add(temperature);
			
			if (temperature >= maxTemperature)
			{
				alarmState = true;
				sigSend (1);//rzuæ alarm ¿e za gor¹co
			}
			if (temperature <= minTemperature)
			{
				alarmState = true;
				sigSend (2);//rzuæ alarm ¿e za zimno
			}
			Thread.sleep(2000);
		}
	}
	public void sigSend (int sigNo)
	{
		switch (sigNo)
		{
		case 1:
		{
			System.out.println("PRZEGRZANIE SYSTEMU!!");
		//TODO sygnal ze za goraco!	
			break;
		}
		case 2:
		{
			System.out.println("PRZECH£ODZENIE SYSTEMU!!");
		//TODO sygnal ze za zimno!
			break;
		}
		}
	}

}
