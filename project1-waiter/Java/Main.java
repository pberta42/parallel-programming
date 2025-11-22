
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Main {

	public final static Lock uvar_lock = new ReentrantLock();
	public final static Lock cond_lock = new ReentrantLock();
	public final static Condition pauzaCond  = cond_lock.newCondition();
	public final static Condition varenieCond  = uvar_lock.newCondition();
	public final static Condition uvareneCond  = uvar_lock.newCondition();
	
	public static final int THREAD_NUM = 5;
	public static final int KUCHAR_NUM = 4;
	
	public static int uvarene = 0;
	public static int uvarene_akt = 0;
	public static int stoj = 0;
	public static int cakaju = 0;
	public static int pauzuju = 0;
	public static int objednane = 0;
	
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		PrimeThread casnik[] = new PrimeThread[THREAD_NUM];
		SecondThread kuchar[] = new SecondThread[KUCHAR_NUM];
		
		System.out.println("Start");
		
		for (int i = 0; i < THREAD_NUM; i++){
			casnik[i] = new PrimeThread(i);
			casnik[i].start();
		}
		
		for (int i = 0; i < KUCHAR_NUM; i++){
			kuchar[i] = new SecondThread(i);
			kuchar[i].start();
		}
		
		try {
			TimeUnit.SECONDS.sleep(30);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		stoj = 1;
		
		for (int i = 0; i < THREAD_NUM; i++){
			try {
				casnik[i].join();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		for (int i = 0; i < KUCHAR_NUM; i++){
			try {
				kuchar[i].join();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		System.out.println("Dokopy sa uvarilo "+uvarene+" jedal");	
	}
}
		