
import java.util.concurrent.TimeUnit;

class Buffer {
   static final int N = 10; //qtde de elementos no buffer

   private int[] buffer; //area de dados compartilhada

   //variaveis de estado
   private int count; //qtde de posicoes ocupadas no buffer
   private int in; //proxima posicao de insercao
   private int out; //proxima posicao de retirada

   // Construtor
   Buffer() {
      this.count = 0;
      this.in = 0;
      this.out = 0;
      this.buffer = new int[N];
   }

   // Insere um item
   public synchronized void insere (int item) {
      while (count == N) {
         try{ wait(); }
         catch (InterruptedException e) { return; }
      }
      buffer[in % N] = item;
      in++;
      count++;
      notifyAll();
   }

   // Remove um item
   public synchronized int remove () {
      int aux;
      while (count == 0) {
         try{ wait(); }
         catch (InterruptedException e) { return -1; }
      }
      aux = buffer[out % N];
      out++;
      count--;
      notifyAll();
      return aux;
   }
}

class Leitor extends Thread {
   private Buffer buf;
   
   //--construtor
   public Leitor(Buffer b) { 
      this.buf = b;
   }

   //--metodo executado pela thread
   public void run() {
      while(true){
         buf.remove();
      }
   }
}

class Escritor extends Thread {
   private Buffer buf;
   private int value;
   
   //--construtor
   public Escritor(Buffer b, int tid) { 
      this.buf = b;
      this.value = tid;
   }

   //--metodo executado pela thread
   public void run() {
      while(true){
         buf.insere(value);
      }
   }
}

//--classe do metodo main
class Programa {
   static final int N = 8;

   public static void main (String[] args) {
      Thread[] readers = new Thread[N];
      Thread[] writers = new Thread[N];

      Buffer buff = new Buffer();
    
      for (int i = 0; i < N; i++) {
         writers[i] = new Escritor(buff, i);
         readers[i] = new Leitor(buff);
      }

      for (int i = 0; i < N; i++) {
         writers[i].start();
         readers[i].start();
      }

      for (int i = 0; i < N; i++) {
         try { writers[i].join(); } 
         catch (InterruptedException e) { return; }
         try { readers[i].join(); } 
         catch (InterruptedException e) { return; }
      }

      System.out.println("Terminou"); 
   }
}
