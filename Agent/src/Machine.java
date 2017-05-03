
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
