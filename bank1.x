program BANK1 {
    version BANK1_VERS{
        int B1_credit(string, int) = 1;
        int B1_debit(string, int) = 2;    
    } = 1;
} = 0x20000002;
