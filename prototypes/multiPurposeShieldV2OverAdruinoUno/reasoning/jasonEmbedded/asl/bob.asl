serialPort(ttyUSB0). /* /dev/ttyUSB0 on Linux. Adjust this belief if necessarry */

!connect.
!helloWorld.

+!connect: serialPort(Port) <- 
	.port(Port); 
	.limit(500); 
	.percepts(open).

+!helloWorld: port(P,Status) <-
	.act(ledRedOn);	    .wait(1000);
	.act(ledRedOff);	.wait(1000);
	
	.act(ledBlueOn);	.wait(1000);
	.act(ledBlueOff);	.wait(1000);

	.act(buzzerOn);	    .wait(1000);
	.act(buzzerOff);	.wait(1000);
	
	.act(rgbWhite);	    .wait(1000);
	.act(rgbGreen);	    .wait(1000);
	.act(rgbBlue);	    .wait(1000);
	.act(rgbRed);	    .wait(1000);
	.act(rgbOff);		.wait(1000);
.

-!helloWorld[error(no_applicable)] <-
	.print("Waiting for connecting..."); .random(R); .wait(5000*R); !helloWorld.


+port(Port,Status) <- .print("The USB port ",Port," is ",Status).

	
+button1(S) <- .print("Btn 1: ",S).
+button2(S) <- .print("Btn 2: ",S).
+hum(H) <- .print("Humidity is ",H).
+temp(T) <- .print("Temperature is ",T).
+buzzer(B)  <- .print("Buzzer is ",B).
+blue(V) <- .print("Blue Led is ",V).
+red(V)  <- .print("Red Led is ",V).
+rgb(V)  <- .print("RGB Led is ",V).
+cmd(C)  <- .print("Remote control CMD is ",C).
+lum(L)  <- .print("Luminosity is ",L).
+pot(P) <- .print("Potenciometer is ",P).

+port(Port,Status) <- .print("The USB port ",Port," is ",Status).