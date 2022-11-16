import java.util.concurrent.Semaphore;

public class TrafficController {

	private Semaphore semaforo = new Semaphore(1); 
	
    public void enterLeft() {
    	try {
    		semaforo.acquire();
    	} catch (InterruptedException e) {
    		e.printStackTrace();
    	}
    	
    }
    public void enterRight() {
    	try {
    		semaforo.acquire();
    	} catch (InterruptedException e) {
    		e.printStackTrace();
    	}
    }
    public void leaveLeft() {
    	semaforo.release();
    }
    public void leaveRight() {
    	semaforo.release();
    }

}