//module sim_isa(
// /*isa data pin*/
//	 input isa_clk,
//	 inout [15:0] isa_data,
//	 input [14:0] isa_addr,
//	 input [6:0] isa_la_addr,
//	 input mem_w,
//	 input mem_r,
//	 input bale,
//	 output  io_chrdy
//);
//
//reg [15:0] isa_data_r;
//reg [15:0] isa_data_w;
//
//assign isa_data = mem_r ? isa_data_r:16'bz;
//
//always @(negedge bale) begin
//	bale_edge = ~bale_edge;
//end
//	
//always @(posedge isa_clk or negedge rst_n) begin
//	if (mem_r == 1'b0 && bale_edge != bale_flag) begin
//		isa_data_r <= isa_data_w;
//		bale_flag <= bale_edge;
//		io_chrdy <= 1'b0;
//	end else if (mem_w == 1'b0 && bale_edge != bale_flag) begin
//		io_chrdy <= 1'b0;
//		bale_flag <= bale_edge;
//		isa_data_w <= isa_data;
//	end else if (io_chrdy == 1'b0) begin
//		io_chrdy <= 1'b1;
//	end
//end
//
//
//
//endmodule

