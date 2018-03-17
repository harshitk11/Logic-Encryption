genvar i,j,k,m,s,l,n,t;
generate
	
	for (i=1, j=1; i<=N/2; i=i+2, j= j+1) begin : loop1
		if ((j%2) == 1) begin						// If j is odd
			if ((midval+j)==(N-2)) begin			// If (midval+j)th ff is the final flip flop of the 1st shifter 
				scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[i]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[midval + j]));			// This is (midval + j)th flipflop
				mux m( .pin0(intercon[midval+(midval/2)+j]), .pin1(out1[i]), .sel(pkey[midval+(midval/2)+j]), .out(muxout[midval+(midval/2)+j]));			// This is (midval + midval/2 + j)th mux
			end
			
			else if ((midval+j)== N) begin			// If (midval+j)th ff is the final flip flop of the 2nd shifter 
				scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[i]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[midval + j]));			// This is (midval + j)th flipflop
				mux m( .pin0(intercon[midval+(midval/2)+j]), .pin1(out1[i]), .sel(pkey[midval+(midval/2)+j]), .out(muxout[midval+(midval/2)+j]));			// This is (midval + midval/2 + j)th mux
			end
			
			else if (((midval+j)%2)==1) begin			// If (midval+j) is odd 
				scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[i]), .in2(intercon[midval + j + 1]), .clk(), .reset(), .mux_sel(), .out(intercon[midval + j]));			// This is (midval + j)th flipflop
				mux m( .pin0(intercon[midval+(midval/2)+j]), .pin1(out1[i]), .sel(pkey[midval+(midval/2)+j]), .out(muxout[midval+(midval/2)+j]));			// This is (midval + midval/2 + j)th mux
			end		
						
			else begin									// If (midval+j) is even
				scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[i]), .in2(intercon[midval + j + 3]), .clk(), .reset(), .mux_sel(), .out(intercon[midval + j]));			// This is (midval + j)th flipflop
				mux m( .pin0(intercon[midval+(midval/2)+j]), .pin1(out1[i]), .sel(pkey[midval+(midval/2)+j]), .out(muxout[midval+(midval/2)+j]));			// This is (midval + midval/2 + j)th mux
			end
		end
		
		
		else begin														// If j is even 
			if ((midval+(midval/2)+j)==(N-2)) begin			// If (midval+(midval/2)+j)th ff is the final flip flop of the 1st shifter 
				scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[i]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[midval+(midval/2)+j]));			// This is (midval+(midval/2)+j)th flipflop
				mux m( .pin0(out1[i]), .pin1(intercon[midval + j]), .sel(pkey[midval+j]), .out(muxout[midval+j]));			// This is (midval + j)th mux
			end
			
			else if ((midval+(midval/2)+j)==(N)) begin			// If (midval+(midval/2)+j)th ff is the final flip flop of the 2nd shifter 
				scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[i]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[midval+(midval/2)+j]));			// This is (midval+(midval/2)+j)th flipflop
				mux m( .pin0(out1[i]), .pin1(intercon[midval + j]), .sel(pkey[midval+j]), .out(muxout[midval+j]));			// This is (midval + j)th mux
			end
			
			else if (((midval+(midval/2)+j)%2)==1) begin			// If (midval+(midval/2)+j) is odd 
				scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[i]), .in2(intercon[midval+(midval/2)+j+1]), .clk(), .reset(), .mux_sel(), .out(intercon[midval+(midval/2)+j]));	// This is (midval + midval/2 + j)th flipflop
				mux m( .pin0(out1[i]), .pin1(intercon[midval + j]), .sel(pkey[midval+j]), .out(muxout[midval+j]));			// This is (midval + j)th mux
			end
			
			else begin													// If (midval+(midval/2)+j) is even
				scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[i]), .in2(intercon[midval+(midval/2)+j+3]), .clk(), .reset(), .mux_sel(), .out(intercon[midval+(midval/2)+j]));	// This is (midval + midval/2 + j)th flipflop
				mux m( .pin0(out1[i]), .pin1(intercon[midval + j]), .sel(pkey[midval+j]), .out(muxout[midval+j]));			// This is (midval + j)th mux
			end
		end
	end
	
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------//

	
	for (l=2, k=1; l<=N/2; l=l+2, k= k+1) begin : loop2
		if ((k%2) == 1) begin						// If k is odd
			if (k==(N-2)) begin			// If k th ff is the final flip flop of the 1st shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[l]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[k]));			// This is k th flipflop
				mux m( .pin0(intercon[(midval/2)+k]), .pin1(out1[l]), .sel(pkey[(midval/2)+k]), .out(muxout[(midval/2)+k]));			// This is (midval/2 + k)th mux
			end
			
			else if (k== N) begin			// If k th ff is the final flip flop of the 2nd shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[l]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[k]));			// This is k th flipflop
				mux m( .pin0(intercon[(midval/2)+k]), .pin1(out1[l]), .sel(pkey[(midval/2)+k]), .out(muxout[(midval/2)+k]));			// This is (midval/2 + k)th mux
			end
			
			else if ((k%2)==1) begin			// If (k) is odd 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[l]), .in2(intercon[k + 1]), .clk(), .reset(), .mux_sel(), .out(intercon[k]));			// This is (k)th flipflop
				mux m( .pin0(intercon[(midval/2)+k]), .pin1(out1[l]), .sel(pkey[(midval/2)+k]), .out(muxout[(midval/2)+k]));			// This is (midval/2 + k)th mux
			end		
						
			else begin									// If (k) is even
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[l]), .in2(intercon[k + 3]), .clk(), .reset(), .mux_sel(), .out(intercon[k]));			// This is (k)th flipflop
				mux m( .pin0(intercon[(midval/2)+k]), .pin1(out1[l]), .sel(pkey[(midval/2)+k]), .out(muxout[(midval/2)+k]));			// This is (midval/2 + k)th mux
			end
		end
		
		
		else begin														// If k is even 
			if (((midval/2)+k)==(N-2)) begin			// If ((midval/2)+k)th ff is the final flip flop of the 1st shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[l]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[(midval/2)+k]));			// This is ((midval/2)+k)th flipflop
				mux m( .pin0(out1[l]), .pin1(intercon[k]), .sel(pkey[k]), .out(muxout[k]));			// This is k th mux
			end
			
			else if (((midval/2)+k)==(N)) begin			// If ((midval/2)+k)th ff is the final flip flop of the 2nd shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[l]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[(midval/2)+k]));			// This is ((midval/2)+k)th flipflop
				mux m( .pin0(out1[l]), .pin1(intercon[k]), .sel(pkey[k]), .out(muxout[k]));			// This is k th mux
			end
			
			else if ((((midval/2)+k)%2)==1) begin			// If ((midval/2)+k) is odd 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[l]), .in2(intercon[(midval/2)+k+1]), .clk(), .reset(), .mux_sel(), .out(intercon[(midval/2)+k]));			// This is ((midval/2)+k)th flipflop
				mux m( .pin0(out1[l]), .pin1(intercon[k]), .sel(pkey[k]), .out(muxout[k]));			// This is k th mux
			end
			
			else begin													// If (midval/2)+k) is even
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[l]), .in2(intercon[(midval/2)+k+3]), .clk(), .reset(), .mux_sel(), .out(intercon[(midval/2)+k]));			// This is ((midval/2)+k)th flipflop
				mux m( .pin0(out1[l]), .pin1(intercon[k]), .sel(pkey[k]), .out(muxout[k]));			// This is k th mux
			end
		end
	end
	
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------//

	for (n=midval+1, m=1; n<=N; n=n+2, m= m+1) begin : loop3
		if ((m%2) == 1) begin						// If m is odd
			if (((midval/2)+m)==(N-2)) begin			// If ((midval/2)+m)th ff is the final flip flop of the 1st shifter 
				scrambler s(.pkey1(pkey[n]), .pkey2(pkey[n+1]), selkey, .out1(out1[n]), .out2(out1[n+1]));
				muxff mf (.in1(out1[n]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[(midval/2)+m]));			// This is ((midval/2)+m)th flipflop
				mux m( .pin0(intercon[m]), .pin1(out1[n]), .sel(pkey[m]), .out(muxout[m]));			// This is m th mux
			end
			
			else if (((midval/2)+m)==(N)) begin			// If ((midval/2)+m)th ff is the final flip flop of the 2nd shifter 
				scrambler s(.pkey1(pkey[n]), .pkey2(pkey[n+1]), selkey, .out1(out1[n]), .out2(out1[n+1]));
				muxff mf (.in1(out1[n]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[(midval/2)+m]));			// This is ((midval/2)+m)th flipflop
				mux m( .pin0(intercon[m]), .pin1(out1[n]), .sel(pkey[m]), .out(muxout[m]));			// This is m th mux
			end
			
			else if ((((midval/2)+m)%2)==1) begin			// If ((midval/2)+m) is odd 
				scrambler s(.pkey1(pkey[n]), .pkey2(pkey[n+1]), selkey, .out1(out1[n]), .out2(out1[n+1]));
				muxff mf (.in1(out1[n]), .in2(intercon[(midval/2)+m+1]), .clk(), .reset(), .mux_sel(), .out(intercon[(midval/2)+m]));			// This is ((midval/2)+m)th flipflop
				mux m( .pin0(intercon[m]), .pin1(out1[n]), .sel(pkey[m]), .out(muxout[m]));			// This is m th mux
			end
			
			else begin													// If (midval/2)+m) is even
				scrambler s(.pkey1(pkey[n]), .pkey2(pkey[n+1]), selkey, .out1(out1[n]), .out2(out1[n+1]));
				muxff mf (.in1(out1[n]), .in2(intercon[(midval/2)+m+3]), .clk(), .reset(), .mux_sel(), .out(intercon[(midval/2)+m]));			// This is ((midval/2)+m)th flipflop
				mux m( .pin0(intercon[m]), .pin1(out1[n]), .sel(pkey[m]), .out(muxout[m]));			// This is m th mux
			end
		end
		
		
		else begin														// If m is even 
			if (m==(N-2)) begin			// If m th ff is the final flip flop of the 1st shifter 
				scrambler s(.pkey1(pkey[n]), .pkey2(pkey[n+1]), selkey, .out1(out1[n]), .out2(out1[n+1]));
				muxff mf (.in1(out1[n]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[m]));			// This is m th flipflop
				mux m( .pin0(out1[n]), .pin1(intercon[(midval/2)+m]), .sel(pkey[(midval/2)+m]), .out(muxout[(midval/2)+m]));			// This is (midval/2 + m)th mux
			end
			
			else if (m== N) begin			// If m th ff is the final flip flop of the 2nd shifter 
				scrambler s(.pkey1(pkey[n]), .pkey2(pkey[n+1]), selkey, .out1(out1[n]), .out2(out1[n+1]));
				muxff mf (.in1(out1[n]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[m]));			// This is m th flipflop
				mux m( .pin0(out1[n]), .pin1(intercon[(midval/2)+m]), .sel(pkey[(midval/2)+m]), .out(muxout[(midval/2)+m]));			// This is (midval/2 + m)th mux
			end
			
			else if ((m%2)==1) begin			// If (m) is odd 
				scrambler s(.pkey1(pkey[n]), .pkey2(pkey[n+1]), selkey, .out1(out1[n]), .out2(out1[n+1]));
				muxff mf (.in1(out1[n]), .in2(intercon[m + 1]), .clk(), .reset(), .mux_sel(), .out(intercon[m]));			// This is (m)th flipflop
				mux m( .pin0(out1[n]), .pin1(intercon[(midval/2)+m]), .sel(pkey[(midval/2)+m]), .out(muxout[(midval/2)+m]));			// This is (midval/2 + m)th mux
			end		
						
			else begin									// If (m) is even
				scrambler s(.pkey1(pkey[n]), .pkey2(pkey[n+1]), selkey, .out1(out1[n]), .out2(out1[n+1]));
				muxff mf (.in1(out1[l]), .in2(intercon[m + 3]), .clk(), .reset(), .mux_sel(), .out(intercon[m]));			// This is (m)th flipflop
				mux m( .pin0(out1[n]), .pin1(intercon[(midval/2)+m]), .sel(pkey[(midval/2)+m]), .out(muxout[(midval/2)+m]));			// This is (midval/2 + m)th mux
			end
		end
	end

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------//	
	
	for (t=1, s=1; t<=N/2; t=t+2, s= s+1) begin : loop4
		if ((s%2) == 1) begin						// If s is odd
			if ((midval+(midval/2)+s)==(N-2)) begin			// If (midval+(midval/2)+s)th ff is the final flip flop of the 1st shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[t]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[midval+(midval/2)+s]));			// This is (midval+(midval/2)+s)th flipflop
				mux m( .pin0(intercon[midval + s]), .pin1(out1[t]), .sel(pkey[midval+s]), .out(muxout[midval+s]));			// This is (midval + s)th mux
			end
			
			else if ((midval+(midval/2)+s)==(N)) begin			// If (midval+(midval/2)+s)th ff is the final flip flop of the 2nd shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[t]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[midval+(midval/2)+s]));			// This is (midval+(midval/2)+s)th flipflop
				mux m( .pin0(intercon[midval + s]), .pin1(out1[t]), .sel(pkey[midval+s]), .out(muxout[midval+s]));			// This is (midval + s)th mux
			end
			
			else if (((midval+(midval/2)+s)%2)==1) begin			// If (midval+(midval/2)+s) is odd 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[t]), .in2(intercon[midval+(midval/2)+s+1]), .clk(), .reset(), .mux_sel(), .out(intercon[midval+(midval/2)+s]));	// This is (midval + midval/2 + s)th flipflop
				mux m( .pin0(intercon[midval + s]), .pin1(out1[t]), .sel(pkey[midval+s]), .out(muxout[midval+s]));			// This is (midval + s)th mux
			end
			
			else begin													// If (midval+(midval/2)+s) is even
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[t]), .in2(intercon[midval+(midval/2)+s+3]), .clk(), .reset(), .mux_sel(), .out(intercon[midval+(midval/2)+s]));	// This is (midval + midval/2 + s)th flipflop
				mux m( .pin0(intercon[midval + s]), .pin1(out1[t]), .sel(pkey[midval+s]), .out(muxout[midval+s]));			// This is (midval + s)th mux
			end			
		end
		
		
		else begin														// If s is even 
			if ((midval+s)==(N-2)) begin			// If (midval+s)th ff is the final flip flop of the 1st shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[t]), .in2(intercon[1]), .clk(), .reset(), .mux_sel(), .out(intercon[midval + s]));			// This is (midval + s)th flipflop
				mux m( .pin0(out1[t]), .pin1(intercon[midval+(midval/2)+s]), .sel(pkey[midval+(midval/2)+s]), .out(muxout[midval+(midval/2)+s]));			// This is (midval + midval/2 + s)th mux
			end
			
			else if ((midval+s)== N) begin			// If (midval+s)th ff is the final flip flop of the 2nd shifter 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[t]), .in2(intercon[3]), .clk(), .reset(), .mux_sel(), .out(intercon[midval + s]));			// This is (midval + s)th flipflop
				mux m( .pin0(out1[t]), .pin1(intercon[midval+(midval/2)+s]), .sel(pkey[midval+(midval/2)+s]), .out(muxout[midval+(midval/2)+s]));			// This is (midval + midval/2 + s)th mux
			end
			
			else if (((midval+s)%2)==1) begin			// If (midval+s) is odd 
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[t]), .in2(intercon[midval + s + 1]), .clk(), .reset(), .mux_sel(), .out(intercon[midval + s]));			// This is (midval + s)th flipflop
				mux m( .pin0(out1[t]), .pin1(intercon[midval+(midval/2)+s]), .sel(pkey[midval+(midval/2)+s]), .out(muxout[midval+(midval/2)+s]));			// This is (midval + midval/2 + s)th mux
			end		
						
			else begin									// If (midval+s) is even
				//scrambler s(.pkey1(pkey[i]), .pkey2(pkey[i+1]), selkey, .out1(out1[i]), .out2(out1[i+1]));
				muxff mf (.in1(out1[t]), .in2(intercon[midval + s + 3]), .clk(), .reset(), .mux_sel(), .out(intercon[midval + s]));			// This is (midval + s)th flipflop
				mux m( .pin0(out1[t]), .pin1(intercon[midval+(midval/2)+s]), .sel(pkey[midval+(midval/2)+s]), .out(muxout[midval+(midval/2)+s]));			// This is (midval + midval/2 + s)th mux
			end
		end
	end

	
endgenerate