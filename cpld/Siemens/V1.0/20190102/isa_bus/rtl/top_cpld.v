module top_cpld(
    //global signal, 50MHZ
    input clk,  
	 /*active low*/
	 input rst_n,
	 
	 /*isa data pin*/
	 inout [15:0] isa_data,
	 output [15:0] isa_addr,
	 output [6:0] isa_la_addr,
	 output mem_w,
	 output mem_r,
	 output bale,
	 input  io_chrdy,
	 
	 /*other control*/
	 output oe,
	 output dir,
	// output led, 
	 /*arm_data*/
	 input addr_sel,
	 input [11:0] arm_addr,
    inout [15:0] arm_data,
	 input arm_cs,
	 output arm_wait,
	 input arm_rd,
	 input arm_wr
);


reg bale_r;
reg arm_wait_r;

reg clk_div_2;
reg clk_div_4;

reg [22:0] isa_addr_r;
reg [1:0] sm;

initial begin
	sm = 2'b00;
end

//assign led = 1'b0;
assign arm_wait = io_chrdy;
//assign bale = 1'b1;
assign bale = bale_r;


assign isa_la_addr = (arm_cs == 1'b0)? isa_addr_r[22:16]:7'b0;
assign isa_addr = (arm_cs == 1'b0)? isa_addr_r[15:0]:16'b0; 

assign isa_data = (arm_wr == 0 )? arm_data:16'bz;
assign arm_data = (arm_rd == 0) ? isa_data:16'bz;
assign mem_r = (sm == 2'b11 & arm_rd == 0) ? 1'b0: 1'b1;
assign mem_w = (sm == 2'b11 & arm_wr== 0) ? 1'b0: 1'b1;
assign oe = 0;
assign dir = (arm_wr == 0)? 1'b1: 1'b0;


always @(posedge clk or negedge rst_n) begin
	if(~rst_n) begin
		isa_addr_r <= 23'h0;
	end
	else begin
		if (addr_sel)begin
			isa_addr_r[11:0] <= arm_addr;
		end else begin
			isa_addr_r[22:12] <= arm_addr[10:0];
		end
	end
end

always @(posedge clk or negedge rst_n) begin
	if(~rst_n) begin
		clk_div_2 <= 0;
	end
	else begin
		clk_div_2 <= ~clk_div_2;
	end
end

always @(posedge clk_div_2 or negedge rst_n) begin
	if(~rst_n) begin
		clk_div_4<= 0;
	end
	else begin
		clk_div_4 <= ~clk_div_4;
	end
end



always @(posedge clk_div_4 or negedge rst_n) begin 
	if (~rst_n) begin
		bale_r <= 0;
		sm <= 2'b00;
	end else if (arm_cs == 1'b0 && bale_r == 1'b0 && sm == 2'b00) begin
		bale_r <= 1'b1;
		sm <= 2'b01;
	end else if (arm_cs == 1'b0 && sm == 2'b01) begin
		bale_r <= 1'b0;
		sm <= 2'b10;
	end else if (arm_cs == 1'b0 && sm == 2'b10) begin
		sm <= 2'b11;
	end else if (arm_cs == 1'b1 ) begin
	   bale_r <= 1'b0;
		sm <= 2'b00;
	end 
end	


endmodule
