program VIRTUAL_BANK{
    version VB_VERS{
        int VB_credit(string, int) = 1;
        int VB_debit(string, int) = 2;
        int VB_transfer(string, string, int) = 3;
    } = 1;
} = 0x20000001;


