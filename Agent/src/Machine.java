import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;

/**
 * Glowna klasa programu symulujacego pomiary temperatury w zbiorniku (maszyna wirtualna)
 * @author Marcin Janeczko
 * @zespol: Aleksander Tym, Marcin Janeczko, Aleksandra Rybak, Katarzyna Romasevska, Bart�omiej Przewo�niak
 * @data: kwiecien-maj 2017
 * Program jest czescia projektu SZPON
 */

public class Machine {

	public static void main(String[] args) throws InterruptedException
	{
		System.out.println("�rodowisko maszyny wirtualnej uruchomione pomy�lnie.\ninicjuj� maszyn� wirtualn�...");
		VirtualMachine VM = new VirtualMachine();
		ArrayList<Integer> params = new ArrayList<Integer>();
		
		//Cykliczne sprawdzanie nowycch polece�
		while (VM.onLine)
		{
			params.clear();
			Thread.sleep(1000);
			String paramsString = new String();
			
			try
			{
				Scanner scanner = new Scanner(new File (System.getProperty("user.dir")+"/src/in.txt"));
				while(scanner.hasNextLine())
				{
					paramsString = scanner.nextLine();
					String[] strAttr = paramsString.split("\\s+");
					for (int i=0;i<strAttr.length; ++i) /**parsowanie lancuchow liczb na int*/
						params.add(Integer.parseInt(strAttr[i]));
				}
				scanner.close();
			}
			catch (FileNotFoundException e)
			{
				System.out.println("B��D OTWARCIA PLIKU Z POLECENIAMI! NIE MOZNA NAWI�ZA� PO��CZENIA Z AGENTEM");
				continue;
			}
			if (params.size() == 1 ) 
				VM.sigRecv(params.get(0));
			else if (params.size()== 3) 
				VM.sigRecv(params.get(0),params.get(1),params.get(2));
			else
				System.out.println("PLIK Z POLECENIAMI JEST USZKODZONY!!!!!!!!!!!11111jedenjedenjedenjedena�ciejeden " + params.size());
		}

		
	}

}
