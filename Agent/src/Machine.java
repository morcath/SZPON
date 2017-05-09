/**
 * Glowna klasa programu symulujacego pomiary temperatury w zbiorniku (maszyna wirtualna)
 * @author Marcin Janeczko
 * @zespol: Aleksander Tym, Marcin Janeczko, Aleksandra Rybak, Katarzyna Romasevska, Bart³omiej PrzewoŸniak
 * @data: kwiecien-maj 2017
 * Program jest czescia projektu SZPON
 */

public class Machine {

	public static void main(String[] args) 
	{
		System.out.println("Œrodowisko maszyny wirtualnej uruchomione pomyœlnie.\n"
				+ "inicjujê maszynê wirtualn¹...");
		VirtualMachine VM = new VirtualMachine();
		try {
			VM.simulate(0, 30);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

}
