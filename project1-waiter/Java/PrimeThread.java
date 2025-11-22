
public class PrimeThread extends Thread {

	private int number;
	private int mojCount;
	
	public PrimeThread(int number){
		this.number = number;
	}
	
	public void run(){
		
		while (Main.stoj != 1){
			
			System.out.println("Zacinam "+mojCount);

			// cesta do kuchyne
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			if (Main.stoj == 1){
				synchronized(Main.cond_lock){Main.cond_lock.unlock();}
				synchronized(Main.uvar_lock){Main.uvar_lock.unlock();}
				synchronized(Main.pauzaCond){Main.pauzaCond.notifyAll();}
				synchronized(Main.varenieCond){Main.varenieCond.notifyAll();}
				synchronized(Main.uvareneCond){Main.uvareneCond.notifyAll();}
				return;
			}
			
			Main.uvar_lock.lock();
			Main.objednane++;
			Main.uvar_lock.unlock();
			
			synchronized(Main.varenieCond){
				Main.varenieCond.notifyAll();
			}
			
			Main.uvar_lock.lock();
			while(Main.uvarene_akt < 1){
				
				try {
					synchronized(Main.uvareneCond){
						Main.uvar_lock.unlock();
						Main.uvareneCond.wait();
						Main.uvar_lock.lock();
					}
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
				if (Main.stoj == 1){
					synchronized(Main.cond_lock){Main.cond_lock.unlock();}
					synchronized(Main.uvar_lock){Main.uvar_lock.unlock();}
					synchronized(Main.pauzaCond){Main.pauzaCond.notifyAll();}
					synchronized(Main.varenieCond){Main.varenieCond.notifyAll();}
					synchronized(Main.uvareneCond){Main.uvareneCond.notifyAll();}
					return;
				}
			}
			
			System.out.println("Odnasam");
			Main.uvarene_akt--;
			
			Main.uvar_lock.unlock();
			
			// cesta naspat
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			if (Main.stoj == 1){
				
				synchronized(Main.cond_lock){Main.cond_lock.unlock();}
				synchronized(Main.uvar_lock){Main.uvar_lock.unlock();}
				synchronized(Main.pauzaCond){Main.pauzaCond.notifyAll();}
				synchronized(Main.varenieCond){Main.varenieCond.notifyAll();}
				synchronized(Main.uvareneCond){Main.uvareneCond.notifyAll();}
				return;
			}
			
			mojCount++;
			
			if (mojCount > 1){
				Main.cond_lock.lock();
				
				System.out.println("Idem cakat na pauzu");
				
				Main.cakaju++;
				
				if (Main.cakaju == 3){
					System.out.println("Pustam");
					synchronized(Main.pauzaCond){
						Main.pauzaCond.notifyAll();
					}
				}
				else {
					while(Main.cakaju < 3){
						try {
							synchronized(Main.pauzaCond){
								Main.cond_lock.unlock();
								Main.pauzaCond.wait();
								Main.cond_lock.lock();
							}
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
						if (Main.stoj == 1){
							synchronized(Main.cond_lock){Main.cond_lock.unlock();}
							synchronized(Main.pauzaCond){Main.pauzaCond.notifyAll();}
							synchronized(Main.varenieCond){Main.varenieCond.notifyAll();}
							synchronized(Main.uvareneCond){Main.uvareneCond.notifyAll();}
							return;
						}
					}	
				}
				
				System.out.println("Pauzujem naozaj");
				Main.pauzuju++;
				
				if (Main.pauzuju==3){
					Main.cakaju = Main.cakaju - 3;
					Main.pauzuju = 0;
				}
				
				synchronized(Main.cond_lock){
					Main.cond_lock.unlock();
				}
				
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
				
				mojCount = 0;
			}

		}
	}
}