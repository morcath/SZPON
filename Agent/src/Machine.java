
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
