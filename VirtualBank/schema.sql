CREATE TABLE account_map (
    account_id TEXT PRIMARY KEY,
    bank_name TEXT NOT NULL
);

INSERT INTO account_map (account_id, bank_name) VALUES ('A12345', 'BANK1');
INSERT INTO account_map (account_id, bank_name) VALUES ('B12345', 'BANK2');