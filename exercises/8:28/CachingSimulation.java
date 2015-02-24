import java.util.Scanner;

public class CachingSimulation
{
    // assume this is VERY costly
    public static int calculateAnswer( int n )
    {
        int sum = 0;
        for ( int i = 1 ; i <= n ; i++ )
        {
            sum += i;
        }
        try { Thread.sleep( 3000 ); } catch ( Exception ex ) { }
        return sum;
    }

    public static void main( String[] args )
    {
        Scanner keyboard = new Scanner( System.in );

        final int cacheSize = 11;

        int[] keys = new int[ cacheSize ];
        int[] values = new int[ cacheSize ];
        // Java initializes these arrays to be all zeroes

        while ( true )
        {
            System.out.print( "Enter n: " );
            int n = keyboard.nextInt();

            int index = n % cacheSize;   // 0-10 index to keys

            if ( keys[ index ] != n )
            {
                // cache miss
                int result = calculateAnswer( n );
                System.out.print( "==> " + result );

                // store the key and the result in cache
                keys[ index ] = n;
                values[ index ] = result;
                System.out.println( " (result has been cached)" );
            }
            else
            {
                // cache hit
                int result = values[ index ];
                System.out.println( "==> " + result + " (cache hit!)" );
            }
        }

    }
}
