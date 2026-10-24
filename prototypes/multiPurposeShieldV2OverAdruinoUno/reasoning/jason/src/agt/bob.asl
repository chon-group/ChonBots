serialPort(ttyUSB0). /* /dev/ttyUSB0 on Linux. Adjust this belief if necessarry */

!connect.
!helloWorld.

+!connect: serialPort(Port) <- 
	.argo.port(Port); 
	.argo.limit(500); 
	.argo.percepts(open).

+!helloWorld: port(P,Status) <-
	.argo.act(ledRedOn);	    .wait(1000);
	.argo.act(ledRedOff);	.wait(1000);
	
	.argo.act(ledBlueOn);	.wait(1000);
	.argo.act(ledBlueOff);	.wait(1000);

	.argo.act(buzzerOn);	    .wait(1000);
	.argo.act(buzzerOff);	.wait(1000);
	
	.argo.act(rgbWhite);	    .wait(1000);
	.argo.act(rgbGreen);	    .wait(1000);
	.argo.act(rgbBlue);	    .wait(1000);
	.argo.act(rgbRed);	    .wait(1000);
	.argo.act(rgbOff);		.wait(1000);
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