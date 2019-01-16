library verilog;
use verilog.vl_types.all;
entity top_cpld is
    port(
        clk             : in     vl_logic;
        rst_n           : in     vl_logic;
        isa_data        : inout  vl_logic_vector(15 downto 0);
        arm_data        : inout  vl_logic_vector(15 downto 0);
        arm_write_op    : in     vl_logic;
        arm_busy_r      : in     vl_logic;
        arm_write_done  : in     vl_logic;
        cpld_write_done : out    vl_logic;
        cpld_write_op   : out    vl_logic;
        cpld_read_finish: out    vl_logic;
        clk_div_2_w     : out    vl_logic;
        clk_div_4_w     : out    vl_logic;
        clk_div_8_w     : out    vl_logic;
        clk_div_16_w    : out    vl_logic;
        clk_div_32_w    : out    vl_logic;
        clk_div_64_w    : out    vl_logic;
        led             : out    vl_logic_vector(3 downto 0)
    );
end top_cpld;
