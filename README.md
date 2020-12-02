# watchpin
use interrupt mode to detect rising or falling edge on gpio and call script via system 

if you want to attach a button to your raspberry pi or orange pi project you will want
to use the interrupt mode to detect a button click.

this program is based on an example program for using the gpio interrupt mode.

example:

        watchpin -p 14 -s '/bin/echo button pressed'
        
        
 xtforever/2020/12/01
 
