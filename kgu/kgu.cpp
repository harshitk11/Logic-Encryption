#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int i,j,k,m,s,l,n,t;
	FILE *fp;

	fp = fopen("kgu.txt","w");
	
	
	int N=8;
	int midval = N/2;

	
	for (i=1, j=1; i<=N/2; i=i+2, j= j+1) { 
		if ((j%2) == 1) {						// If j is odd
			if ((midval+j)==(N-2)) {			// If (midval+j)th ff is the final flip flop of the 1st shifter 
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",i,i+1,i,i+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n", i, midval+j);			// This is (midval + j)th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",midval+(midval/2)+j,i,midval+(midval/2)+j,midval+(midval/2)+j);			// This is (midval + midval/2 + j)th mux
			}
			
			else if ((midval+j)== N) {			// If (midval+j)th ff is the final flip flop of the 2nd shifter 
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",i,i+1,i,i+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n", i, midval+j);			// This is (midval + j)th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",midval+(midval/2)+j,i,midval+(midval/2)+j,midval+(midval/2)+j);			// This is (midval + midval/2 + j)th mux
			}
			
			else if (((midval+j)%2)==1) {			// If (midval+j) is odd 
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",i,i+1,i,i+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n", i, midval + j + 1, midval+j);			// This is (midval + j)th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",midval+(midval/2)+j,i,midval+(midval/2)+j,midval+(midval/2)+j);			// This is (midval + midval/2 + j)th mux
			}		
						
			else {									// If (midval+j) is even
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",i,i+1,i,i+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n", i, midval + j + 3, midval+j);			// This is (midval + j)th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",midval+(midval/2)+j,i,midval+(midval/2)+j,midval+(midval/2)+j);			// This is (midval + midval/2 + j)th mux
			}
		}
		
		
		else {														// If j is even 
			if ((midval+(midval/2)+j)==(N-2)) {			// If (midval+(midval/2)+j)th ff is the final flip flop of the 1st shifter 
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",i,i+1,i,i+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",i,midval+(midval/2)+j);			// This is (midval+(midval/2)+j)th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",i,midval + j,midval + j,midval + j);			// This is (midval + j)th mux
			}
			
			else if ((midval+(midval/2)+j)==(N)) {			// If (midval+(midval/2)+j)th ff is the final flip flop of the 2nd shifter 
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",i,i+1,i,i+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",i,midval+(midval/2)+j);			// This is (midval+(midval/2)+j)th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",i,midval + j,midval + j,midval + j);			// This is (midval + j)th mux
			}
			
			else if (((midval+(midval/2)+j)%2)==1) {			// If (midval+(midval/2)+j) is odd 
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",i,i+1,i,i+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",i,midval+(midval/2)+j+1,midval+(midval/2)+j);	// This is (midval + midval/2 + j)th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",i,midval + j,midval + j,midval + j);			// This is (midval + j)th mux
			}
			
			else {													// If (midval+(midval/2)+j) is even
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",i,i+1,i,i+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",i,midval+(midval/2)+j+3,midval+(midval/2)+j);	// This is (midval + midval/2 + j)th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",i,midval + j,midval + j,midval + j);			// This is (midval + j)th mux
			}
		}
	}
	
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------//

	
	for (l=2, k=1; l<=N/2; l=l+2, k= k+1) { 
		if ((k%2) == 1) {						// If k is odd
			if (k==(N-2)) {			// If k th ff is the final flip flop of the 1st shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",l,k);			// This is k th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",(midval/2)+k,l,(midval/2)+k,(midval/2)+k);			// This is (midval/2 + k)th mux
			}
			
			else if (k== N) {			// If k th ff is the final flip flop of the 2nd shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",l,k);			// This is k th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",(midval/2)+k,l,(midval/2)+k,(midval/2)+k);			// This is (midval/2 + k)th mux
			}
			
			else if ((k%2)==1) {			// If (k) is odd 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",l,k+1,k);			// This is (k)th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",(midval/2)+k,l,(midval/2)+k,(midval/2)+k);			// This is (midval/2 + k)th mux
			}		
						
			else {									// If (k) is even
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",l,k+3,k);			// This is (k)th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",(midval/2)+k,l,(midval/2)+k,(midval/2)+k);			// This is (midval/2 + k)th mux
			}
		}
		
		
		else {														// If k is even 
			if (((midval/2)+k)==(N-2)) {			// If ((midval/2)+k)th ff is the final flip flop of the 1st shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",l,(midval/2)+k);			// This is ((midval/2)+k)th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",l,k,k,k);			// This is k th mux
			}
			
			else if (((midval/2)+k)==(N)) {			// If ((midval/2)+k)th ff is the final flip flop of the 2nd shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",l,(midval/2)+k);			// This is ((midval/2)+k)th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",l,k,k,k);			// This is k th mux
			}
			
			else if ((((midval/2)+k)%2)==1) {			// If ((midval/2)+k) is odd 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",l,(midval/2)+k+1,(midval/2)+k);			// This is ((midval/2)+k)th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",l,k,k,k);			// This is k th mux
			}
			
			else {													// If (midval/2)+k) is even
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",l,(midval/2)+k+3,(midval/2)+k);			// This is ((midval/2)+k)th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",l,k,k,k);			// This is k th mux
			}
		}
	}
	
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------//

	for (n=midval+1, m=1; n<=N; n=n+2, m= m+1) { 
		if ((m%2) == 1) {						// If m is odd
			if (((midval/2)+m)==(N-2)) {			// If ((midval/2)+m)th ff is the final flip flop of the 1st shifter 
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",n,n+1,n,n+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",n,(midval/2)+m);			// This is ((midval/2)+m)th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",m,n,m,m);			// This is m th mux
			}
			
			else if (((midval/2)+m)==(N)) {			// If ((midval/2)+m)th ff is the final flip flop of the 2nd shifter 
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",n,n+1,n,n+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",n,(midval/2)+m);			// This is ((midval/2)+m)th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",m,n,m,m);			// This is m th mux
			}
			
			else if ((((midval/2)+m)%2)==1) {			// If ((midval/2)+m) is odd 
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",n,n+1,n,n+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",n,(midval/2)+m+1,(midval/2)+m);			// This is ((midval/2)+m)th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",m,n,m,m);			// This is m th mux
			}
			
			else {													// If (midval/2)+m) is even
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",n,n+1,n,n+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",n,(midval/2)+m+3,(midval/2)+m);			// This is ((midval/2)+m)th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",m,n,m,m);			// This is m th mux
			}
		}
		
		
		else {														// If m is even 
			if (m==(N-2)) {			// If m th ff is the final flip flop of the 1st shifter 
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",n,n+1,n,n+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",n,m);			// This is m th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",n,(midval/2)+m,(midval/2)+m,(midval/2)+m);			// This is (midval/2 + m)th mux
			}
			
			else if (m== N) {			// If m th ff is the final flip flop of the 2nd shifter 
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",n,n+1,n,n+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",n,m);			// This is m th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",n,(midval/2)+m,(midval/2)+m,(midval/2)+m);			// This is (midval/2 + m)th mux
			}
			
			else if ((m%2)==1) {			// If (m) is odd 
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",n,n+1,n,n+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",n,m+1,m);			// This is (m)th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",n,(midval/2)+m,(midval/2)+m,(midval/2)+m);			// This is (midval/2 + m)th mux
			}		
						
			else {									// If (m) is even
				fprintf(fp,"scrambler s(.pkey1(pkey[%d]), .pkey2(pkey[%d]), selkey, .out1(out1[%d]), .out2(out1[%d]))\n",n,n+1,n,n+1);
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",n,m+3,m);			// This is (m)th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",n,(midval/2)+m,(midval/2)+m,(midval/2)+m);			// This is (midval/2 + m)th mux
			}
		}
	}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------//	
	
	for (t=midval+2, s=1; t<=N; t=t+2, s= s+1) { 
		if ((s%2) == 1) {						// If s is odd
			if ((midval+(midval/2)+s)==(N-2)) {			// If (midval+(midval/2)+s)th ff is the final flip flop of the 1st shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",t,midval+(midval/2)+s);			// This is (midval+(midval/2)+s)th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",midval+s,t,midval+s,midval+s);			// This is (midval + s)th mux
			}
			
			else if ((midval+(midval/2)+s)==(N)) {			// If (midval+(midval/2)+s)th ff is the final flip flop of the 2nd shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",t,midval+(midval/2)+s);			// This is (midval+(midval/2)+s)th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",midval+s,t,midval+s,midval+s);			// This is (midval + s)th mux
			}
			
			else if (((midval+(midval/2)+s)%2)==1) {			// If (midval+(midval/2)+s) is odd 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",t,midval+(midval/2)+s+1,midval+(midval/2)+s);	// This is (midval + midval/2 + s)th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",midval+s,t,midval+s,midval+s);			// This is (midval + s)th mux
			}
			
			else {													// If (midval+(midval/2)+s) is even
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",t,midval+(midval/2)+s+3,midval+(midval/2)+s);	// This is (midval + midval/2 + s)th flipflop
				fprintf(fp,"mux m( .pin0(intercon[%d]), .pin1(out1[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",midval+s,t,midval+s,midval+s);			// This is (midval + s)th mux
			}			
		}
		
		
		else {														// If s is even 
			if ((midval+s)==(N-2)) {			// If (midval+s)th ff is the final flip flop of the 1st shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",t,midval+s);			// This is (midval + s)th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",t,midval+(midval/2)+s,midval+(midval/2)+s,midval+(midval/2)+s);			// This is (midval + midval/2 + s)th mux
			}
			
			else if ((midval+s)== N) {			// If (midval+s)th ff is the final flip flop of the 2nd shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",t,midval+s);			// This is (midval + s)th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",t,midval+(midval/2)+s,midval+(midval/2)+s,midval+(midval/2)+s);			// This is (midval + midval/2 + s)th mux
			}
			
			else if (((midval+s)%2)==1) {			// If (midval+s) is odd 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",t,midval+s+1,midval+s);			// This is (midval + s)th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",t,midval+(midval/2)+s,midval+(midval/2)+s,midval+(midval/2)+s);			// This is (midval + midval/2 + s)th mux
			}		
						
			else {									// If (midval+s) is even
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				fprintf(fp,"muxff mf (.in1(out1[%d]), .in2(intercon[%d]), .clk(), .reset(), .mux_sel(), .out(intercon[%d]))\n",t,midval+s+3,midval+s);			// This is (midval + s)th flipflop
				fprintf(fp,"mux m( .pin0(out1[%d]), .pin1(intercon[%d]), .sel(pkey[%d]), .out(muxout[%d]))\n",t,midval+(midval/2)+s,midval+(midval/2)+s,midval+(midval/2)+s);			// This is (midval + midval/2 + s)th mux
			}
		}
	}

fclose(fp);
	
return 0;

	
}