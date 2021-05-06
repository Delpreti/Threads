
import java.util.*;

class V {

    private int[] v;

    // Construtor
    public V(int size) { 
        this.v = new int[size];
        Random rand = new Random();
        for (int i = 0; i < size; i++) {
            this.v[i] = rand.nextInt() % 10;
        }
    }

    // Construtor de copia
    public V(V vet) { 
        this.v = new int[vet.size()];
        for (int i = 0; i < vet.size(); i++) {
            this.v[i] = vet.get(i);
        }
    }

    public int size() {
        return this.v.length;
    }

    public synchronized void inc(int element) { 
        this.v[element]++; 
    }

    public synchronized int[] get() { 
        return this.v; 
    }

    public int get(int element) {
        return this.v[element];
    }

}

class Incrementer extends Thread {

   private int id;
   private int numThreads;
   V vetor;
  
   // Construtor
   public Incrementer(int tid, int numT, V vettor) { 
      this.id = tid;
      this.numThreads = numT;
      this.vetor = vettor;
   }

   public void run() {
      for (int i = this.id; i < this.vetor.size(); i += this.numThreads) {
         this.vetor.inc(i);  
      }
   }
}

class Application {

    public static void main (String[] args) {

        // Leitura da entrada
        Scanner scan = new Scanner(System.in);
        System.out.print("Insira o numero de Threads: ");  
        final int nThreads = scan.nextInt();
        System.out.print("Insira o tamanho do vetor: ");  
        final int vSize = scan.nextInt();

        // Inicializacao
        Thread[] threads = new Thread[nThreads];
        V vect = new V(vSize);
        V vect_copy = new V(vect);

        // Cria e inicia as threads da aplicacao
        for (int i = 0; i < threads.length; i++) {
            threads[i] = new Incrementer(i, nThreads, vect);
        }
        for (int i = 0; i < threads.length; i++) {
            threads[i].start();
        }

        // Aguarda o termino das threads
        for (int i = 0; i < threads.length; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                return;
            }
        }

        // Verificacao do resultado
        int j;
        for (j = 0; j < vect.size(); j++) {
            if (vect_copy.get(j) + 1 != vect.get(j)) {
                System.out.println("Programa com defeito.");
                break;
            }
        }
        if (j == vect.size()) {
            System.out.println("Programa executado com sucesso.");
        }
    }
}
