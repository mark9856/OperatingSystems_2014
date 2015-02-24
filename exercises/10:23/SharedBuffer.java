import java.util.Vector;

// Example of the Producer-Consumer Problem using Java threads
//
// Producer thread adds integers 1, 2, 3, etc. without bound.
// Consumer threads read from buffer.

public class SharedBuffer
{
  // shared buffer of 8 characters
  public int maxSize = 8;
  public Vector buffer = new Vector( maxSize );
  public int i = 1;

  public void produceMessage() {
    if ( buffer.size() < maxSize ) {
      buffer.addElement( i );
      System.out.println( "PRODUCER: added " + i );
      i++;
    }
  }

  public synchronized void consumeMessage( long threadId ) {
    if ( buffer.size() > 0 ) {
      int n = (Integer)buffer.firstElement();
      buffer.removeElement( n );
      System.out.println( "CONSUMER " + threadId + ": removed " + n );
    }
  }

  public static void main( String[] args ) {
    SharedBuffer program = new SharedBuffer();
    program.doit();
  }

  public void doit() {
    Producer p = new Producer();
    Consumer c1 = new Consumer();
    Consumer c2 = new Consumer();
    Consumer c3 = new Consumer();
    p.start();
    c1.start();
    c2.start();
    c3.start();
  }

  class Producer extends Thread {
    public void run() {
      while ( true ) {
        produceMessage();
        try { Thread.sleep( 1000 ); }
        catch( InterruptedException ex ) {  }
      }
    }
  }

  class Consumer extends Thread {
    public void run() {
      while ( true ) {
        consumeMessage( this.getId() );
//        try { Thread.sleep( 1000 ); }
//        catch( InterruptedException ex ) {  }
      }
    }
  }
}
