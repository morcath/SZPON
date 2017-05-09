/**
 * Glowna klasa programu symulujacego pomiary temperatury w zbiorniku (maszyna wirtualna)
 * @author Marcin Janeczko
 * @zespol: Aleksander Tym, Marcin Janeczko, Aleksandra Rybak, Katarzyna Romasevska, Bart�omiej Przewo�niak
 * @data: kwiecien-maj 2017
 * Program jest czescia projektu SZPON
 */

public class Machine {

	public static void main(String[] args) 
	{
		System.out.println("�rodowisko maszyny wirtualnej uruchomione pomy�lnie.\n"
				+ "inicjuj� maszyn� wirtualn�...");
		VirtualMachine VM = new VirtualMachine();
		try {
			VM.simulate(0, 30);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

}
