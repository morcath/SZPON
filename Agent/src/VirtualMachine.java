/**
 * Klasa wykonawcza wirtualnej maszyny
 * @author Marcin Janeczko
 * @zespol: Aleksander Tym, Marcin Janeczko, Aleksandra Rybak, Katarzyna Romasevska, Bart�omiej Przewo�niak
 * @data: kwiecien-maj 2017
 * Program jest czescia projektu SZPON
 */

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Random;

import javax.swing.*;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.awt.Font;

public class VirtualMachine {
	JFrame mainFrame;
	JPanel panel = new JPanel();
	JButton exitB;
	JButton overheatB;
	JButton overcoolB;
	JButton extremeOvercoolB;
	JButton extremeOverheatB;
	JButton cancelAnomalyB;
	JLabel textLabel1;
	JLabel temperatureLabel;
	JLabel alertLabel;
	
	int temperature; //zmienna trzymajaca chwilowa temperature
	int tempRand; //losowa zmienna odpowiadajaca za wysokosc temperatry w nastepnej iteracji
	int lT, hT; //zmienne trzymajace poziomy temperatury
	Random generator = new Random();
	boolean overheat = false; //zmienna symulujaca nadmierne grzanie systemu - system bedzie sie ogrzewac az do wprowadzenia progu
	boolean extremeOverheat = false; //zmienna symulujaca ekstremalne grzanie systemu - system bedzie sie ogrzewac mimo chlodzenia
	boolean overcool = false; //zmienna symulujaca nadmierne chlodzenia systemu - system bedzie sie chlodzic az do wprowadzenia progu
	boolean extremeOvercool = false; //zmienna symulujaca ekstremalne chlodzenie systemu - system bedzie sie chlodzic mimo chlodzenia
	boolean coolingSig = false; //otrzymano sygnal nakazujacy chlodzenie
	boolean heatingSig = false; //otrzymano sygnal nakazujacy grzanie
	boolean init = true; //zmienna sygnalizujaca inicjalizacje (pierwsza iteracje) pomiaru, do resetu danych
	boolean hotAlarmState = false; //czy jestesmy w stanie alarmu bo za cieplo
	boolean coldAlarmState = false; //czy jestesmy w stanie alarmu bo za zimno
	boolean heating = false; // czy dostalismy polecenie grzania
	boolean cooling = false; //czy dostalismy polecenie chlodzenia
	public boolean onLine = true; //czy system jest uruchominy
	boolean working = false; //czy maszyna teraz mierzy
	boolean fileReady = false; //czy mamy gotowy pomiar
	
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
		
		panel.setLayout(null);
		
		exitB = new JButton ("EXIT");
		exitB.setBounds(556, 300, 214, 40);
		exitB.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent arg0) {
				mainFrame.dispose();
				onLine = false;
				init = false;
				temperaturesList.clear();
		}});
		panel.add(exitB);
		
		overheatB = new JButton("OVERHEATING");
		overheatB.setBounds(327, 199, 214, 40);
		overheatB.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent arg0) {
				overheat = true;
		}});
		panel.add(overheatB);
		
		extremeOverheatB = new JButton ("EXTREME OVERHEATING");
		extremeOverheatB.setBounds(556, 199, 214, 40);
		extremeOverheatB.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent arg0) {
				extremeOverheat = true;
		}});
		panel.add(extremeOverheatB);
		
		overcoolB = new JButton("OVERCOOLING");
		overcoolB.setBounds(327, 146, 214, 40);
		overcoolB.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent arg0) {
				overcool = true;
		}});
		panel.add(overcoolB);
		
		extremeOvercoolB = new JButton("EXTREME OVERCOOLING");
		extremeOvercoolB.setBounds(556, 146, 214, 40);
		extremeOvercoolB.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent arg0) {
				extremeOvercool = true;
		}});
		panel.add(extremeOvercoolB);
		
		cancelAnomalyB = new JButton("CANCEL ALL [EXTREME]OVERHEATING/COOLING SIMULATIONS");
		cancelAnomalyB.setBounds(327, 247, 443, 40);
		cancelAnomalyB.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent arg0) {
				extremeOvercool = false;
				extremeOverheat = false;
				overcool = false;
				overheat = false;
		}});
		panel.add(cancelAnomalyB);
		
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
		
		mainFrame.getContentPane().add(panel);
		mainFrame.setVisible(true);
	}
	public void simulate(int minTemperature, int maxTemperature)
	{
		if (init)
		{
			temperaturesList.clear();
			temperature = generator.nextInt(4)+((maxTemperature+minTemperature)/2)-2;
			temperaturesList.add(temperature);
			init = false;
			working = true;
			fileReady = false;
			lT = minTemperature;
			hT = maxTemperature;
		}
	//warunki na zmiany temperatur
		if (extremeOverheat) //czy jest symulowane grzanie mimo wszystko
			++temperature;
		else if (extremeOvercool) //czy jest symulowane grzanie mimo wszystko
			--temperature;
		else if (heating) //jak nie to czy jest wlaczone chlodzenie lub grzanie przez serwer
			++ temperature;
		else if (cooling)
			-- temperature;
		else if (overheat) //jak nie to czy jest symulowane zwykle grzanie
			++temperature;
		else if (overcool) //jak nie to czy jest symulowane zwykle chlodzenie
			--temperature;
		else //jak nie to symulujemy normalnie zmiane
		{
			tempRand = generator.nextInt(100);
			if (tempRand > 60)
				++temperature;
			else if (tempRand < 40)
				--temperature;
			else;
				
		}
	//wklepanie nowej temperatury do listy	
		temperaturesList.add(temperature);
	//potencjalne wywolanie alarmu	
		if (temperature >= maxTemperature+1)
		{
			hotAlarmState = true;
			sigSend (1);//rzuc alarm ze za goraco
		}
		else if (temperature <= minTemperature-1)
		{
			coldAlarmState = true;
			sigSend (2);//rzuc alarm ze za zimno
		}
		else if (hotAlarmState && temperature < maxTemperature -3)
		{
			cooling = false;
			alertLabel.setText("");
			hotAlarmState = false;
			sigSend(3);
		}
		else if (coldAlarmState && temperature > minTemperature + 3)
		{
			heating = false;
			alertLabel.setText("");
			coldAlarmState = false;
			sigSend (3);
		}
		
	//WYSWIETLANIE
		if (hotAlarmState || coldAlarmState)
			temperatureLabel.setForeground(Color.RED);
		else
			temperatureLabel.setForeground(Color.BLACK);
		
		System.out.println("Aktualny pomiar: " + temperature);
		temperatureLabel.setText(""+temperature);
	}
	
	
	public void sigSend (int sigNo)
	{
		switch (sigNo)
		{
		case 1: //za wysoka temperatura
		{
			System.out.println("PRZEGRZANIE SYSTEMU!!");
			alertLabel.setText("PRZEGRZANIE SYSTEMU!");
			try{
			    PrintWriter writer = new PrintWriter(System.getProperty("user.dir")+"/out", "UTF-8");
			    writer.println("1");
			    writer.close();
			} catch (IOException e) {
			   e.printStackTrace();
			}
			break;
		}
		case 2: //za niska temperatura
		{
			System.out.println("PRZECHLODZENIE SYSTEMU!!");
			alertLabel.setText("PRZECHLODZENIE SYSTEMU!");
			try{
			    PrintWriter writer = new PrintWriter(System.getProperty("user.dir")+"/out", "UTF-8");
			    writer.println("2");
			    writer.close();
			} catch (IOException e) {
			   e.printStackTrace();
			}
			break;
		}
		case 3: //ustabilizowana temperatura
		{
			try{
				overheat = overcool = extremeOverheat = extremeOvercool = false;
			    PrintWriter writer = new PrintWriter(System.getProperty("user.dir")+"/out", "UTF-8");
			    writer.println("3");
			    writer.close();
			} catch (IOException e) {
			   e.printStackTrace();
			}
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
			if (!working)
				System.out.println("Wywolano chlodzenie nie wywolujac pomiaru. Moze to swiadczyc o uszkodzeniu serwera lub pliku z poleceniami");
			else
			{
				cooling = true;
				simulate (lT,hT);
			}
			break;
		}
		case 2: //otrzymano rozkaz grzania
		{
			if (!working)
				System.out.println("Wywolano grzanie nie wywolujac pomiaru. Moze to swiadczyc o uszkodzeniu serwera lub pliku z poleceniami");
			else
			{
				heating = true;
				simulate (lT,hT);
			}
			break;
		}
		case 3: //otrzymano sygnal rozpocz�cia pomiar�w
		{
				System.out.println("NIE PODANO PARAMETROW POMIARU!!!");
				alertLabel.setText("NIE PODANO PARAMETROW POMIARU!!!");
			break;
		}
		case 4: //otrzymano sygnal zakonczenia pomiarow
		{
			working = false;
			init = true;
			if (!fileReady){
				//generowanie pliku xml
				try {
			        DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			        DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
			        Document doc = dBuilder.newDocument();
			        // root element
			        Element root = doc.createElement("plik");
			        doc.appendChild(root);
			        
			        //dane element
			       
			        Element pomiar = doc.createElement("Pomiar");
			        root.appendChild(pomiar);
			        
			     
			        
			       for (int i=0; i<temperaturesList.size(); ++i)
			       {	Element dane = doc.createElement("Dane");
			       		dane.appendChild(doc.createTextNode(String.valueOf(temperaturesList.get(i))));
			       		pomiar.appendChild(dane);
			       }
			       
			       
			    // Save the document to the disk file
			       TransformerFactory tranFactory = TransformerFactory.newInstance();
			       Transformer aTransformer = tranFactory.newTransformer();
			    // format the XML nicely
			       aTransformer.setOutputProperty(OutputKeys.ENCODING, "ISO-8859-1");
	
			       aTransformer.setOutputProperty(
			               "{http://xml.apache.org/xslt}indent-amount", "4");
			       aTransformer.setOutputProperty(OutputKeys.INDENT, "yes");
			       
			       DOMSource source = new DOMSource(doc);
			       try {
			           // location and name of XML file you can change as per need
			           FileWriter fos = new FileWriter(System.getProperty("user.dir")+"/data.xml");
			           StreamResult result = new StreamResult(fos);
			           aTransformer.transform(source, result);
	
			       } catch (IOException e) {
	
			           e.printStackTrace();
			       
			       }
			       StreamResult consoleResult = new StreamResult(System.out);
			       aTransformer.transform(source, consoleResult);
				}catch (Exception e) {
			        e.printStackTrace();
			     }
				//koniec generowania
				System.out.println("Wygenerowano plik xml");
				fileReady = true;
				try{
				    PrintWriter writer = new PrintWriter(System.getProperty("user.dir")+"/out", "UTF-8");
				    writer.println("4");
				    writer.close();
				} catch (IOException e) {
				   e.printStackTrace();
				}
			}
			break;
		}
		default:;
		}
	}
	
	public void sigRecv(int sigNo, int minTemp, int maxTemp)
	{
		if (sigNo == 3)
			simulate(minTemp, maxTemp);
		else
			sigRecv (sigNo);
	}

}
 
