
public class SecondThread extends Thread {

	private int number;
	
	public SecondThread(int number){
		this.number = number;
	}
	
	public void run(){
		
		while (Main.stoj != 1){
			Main.uvar_lock.lock();
			while(Main.objednane <1){
				try {
					synchronized(Main.varenieCond){
						Main.uvar_lock.unlock();
						Main.varenieCond.wait();
						Main.uvar_lock.lock();
					}
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
				if (Main.stoj == 1){
					synchronized(Main.uvar_lock){Main.uvar_lock.unlock();}
					synchronized(Main.pauzaCond){Main.pauzaCond.notifyAll();}
					synchronized(Main.varenieCond){Main.varenieCond.notifyAll();}
					synchronized(Main.uvareneCond){Main.uvareneCond.notifyAll();}
					return;
				}
			}
			
			Main.objednane--;
			Main.uvar_lock.unlock();
			
			System.out.println("Varim");
			try {
				Thread.sleep(2000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			if (Main.stoj == 1){
				synchronized(Main.pauzaCond){Main.pauzaCond.notifyAll();}
				synchronized(Main.varenieCond){Main.varenieCond.notifyAll();}
				synchronized(Main.uvareneCond){Main.uvareneCond.notifyAll();}
				return;
			}
			
			Main.uvar_lock.lock();	
			Main.uvarene_akt++;
			Main.uvarene++;
			Main.uvar_lock.unlock();	
			synchronized(Main.uvareneCond){
				Main.uvareneCond.notifyAll();
			}
		}
		return;
	}
}
