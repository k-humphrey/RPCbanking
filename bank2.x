program BANK2 {
    version BANK2_VERS {
        int B2_credit(string, int) = 1;
        int B2_debit(string, int) = 2;
    } = 1;
} = 0x20000003;