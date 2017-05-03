import java.awt.Color;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.util.ArrayList;
import java.util.Random;

import javax.swing.*;
import java.awt.Font;
import java.awt.GridLayout;

public class VirtualMachine {
	JFrame mainFrame;
	JPanel panel = new JPanel();
	JButton exitB;
	JButton overheatB;
	JButton extremeOverheatB;
	JLabel textLabel1;
	JLabel temperatureLabel;
	JLabel alertLabel;
	
	boolean overheat = false; //zmienna symulujaca nadmierne grzanie systemu - system bedzie sie ogrzewac az do wprowadzenia progu
	boolean extremeOverheat = false; //zmienna symulujaca ekstremalne grzanie systemu - system bedzie sie ogrzewac mimo chlodzenia
	boolean coolingSig = false; //otrzymano sygnal nakazujacy chlodzenie
	boolean heatingSig = false; //otrzymano sygnal nakazujacy grzanie
	boolean measure = true; //otrzymano sygnal nakazujacy pomiar
	boolean alarmState = false; //czy jestesmy w stanie alarmu
	boolean heating = false; // czy dostalismy polecenie grzania
	boolean cooling = false; //czy dostalismy polecenie chlodzenia
	
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
		
		mainFrame.setBounds((screenWidth/2)-400, (screenHeight/2)-200, 800, 400);
		panel = new JPanel();
		
		System.out.println("Helo");
		panel.setLayout(null);
		
		exitB = new JButton ("Wyjœcie");
		exitB.setBounds(595, 300, 175, 40);
		panel.add(exitB);
		
		overheatB = new JButton("Przegrzanie");
		overheatB.setBounds(595, 146, 175, 40);
		panel.add(overheatB);
		
		extremeOverheatB = new JButton ("Awaryjne przegrzanie");
		extremeOverheatB.setBounds(595, 199, 175, 40);
		panel.add(extremeOverheatB);
		
		textLabel1 = new JLabel ("Aktualna temperatura: ");
		textLabel1.setBounds(595, 13, 175, 31);
		textLabel1.setFont(new Font("Tahoma", Font.PLAIN, 17));
		panel.add(textLabel1);
		
		temperatureLabel = new JLabel ("0");
		temperatureLabel.setHorizontalAlignment(SwingConstants.CENTER);
		temperatureLabel.setBounds(595, 57, 175, 65);
		temperatureLabel.setFont(new Font("Tahoma", Font.PLAIN, 50));
		panel.add(temperatureLabel);
		
		alertLabel = new JLabel("");
		alertLabel.setLocation(595, 118);
		alertLabel.setSize(175, 21);
		alertLabel.setFont(new Font("Tahoma", Font.PLAIN, 15));
		alertLabel.setHorizontalAlignment(SwingConstants.CENTER);
		alertLabel.setForeground(Color.RED);
		panel.add(alertLabel);
		
		//TODO: dodaæ guziki, ogarn¹æ wykresy itp, chcemy: Label z aktualnym pomiarem, 
		//wykres liniowy (mo¿e?), wykres s³upkowy
		mainFrame.getContentPane().add(panel);
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
			if (heating)
				++ temperature;
			else if (cooling && !extremeOverheat)
				-- temperature;
			else if (overheat || extremeOverheat)
				++temperature;
			else
			{
				tempRand = generator.nextInt(100);
				if (tempRand > 60)
					++temperature;
				else if (tempRand < 40)
					--temperature;
				else /*if (tempRand > 45 && tempRand < 55)*/;
				/*else
				{
					if (1==1)//funkcja uzale¿niaj¹ca wynik losowania od akt temp
						++temperature;
					else
						--temperature;
				}*/
					
			}
			temperaturesList.add(temperature);
			
			if (temperature >= maxTemperature)
			{
				alarmState = true;
				sigSend (1);//rzuæ alarm ¿e za gor¹co
			}
			else if (temperature <= minTemperature)
			{
				alarmState = true;
				sigSend (2);//rzuæ alarm ¿e za zimno
			}
			else if (alarmState && (temperature < maxTemperature - 3 || temperature > minTemperature + 3))//TODO spierdolony warunek
			{
				heating = cooling = alarmState = false;
				alertLabel.setText("");
				alarmState = false;
				sigSend (3);
			}
			
			//WYŒWIETLANIE
			if (alarmState)
				temperatureLabel.setForeground(Color.RED);
			else
				temperatureLabel.setForeground(Color.BLACK);
			
			System.out.println(temperature);
			temperatureLabel.setText(""+temperature);
			Thread.sleep(250);
		}
	}
	
	
	public void sigSend (int sigNo)
	{
		switch (sigNo)
		{
		case 1: //za wysoka temperatura
		{
			System.out.println("PRZEGRZANIE SYSTEMU!!");
			alertLabel.setText("PRZEGRZANIE SYSTEMU!");
		//TODO Wyslanie sygnalu ze za goraco!	
			break;
		}
		case 2: //za niska temperatura
		{
			System.out.println("PRZECH£ODZENIE SYSTEMU!!");
			alertLabel.setText("PRZECH£ODZENIE SYSTEMU!");
		//TODO Wyslanie sygnalu ze za zimno!
			break;
		}
		case 3: //ustabilizowana temperatura
		{
			//TODO Wyslanie sygnalu o ustabilizowanej temperaturze
			break;
		}
		}
	}
	
	
	public void sigRecv(int sigNo)
	{
		switch (sigNo)
		{
		case 1: //otrzymano rozkaz chlodzenia
		{
			cooling = true;
			break;
		}
		case 2: //otrzymano rozkaz grzania
		{
			heating = true;
			break;
		}
		}
	}

}
