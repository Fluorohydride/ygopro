BEGIN TRANSACTION;
--Add rule_code column
--step 1: add new column
ALTER TABLE datas ADD COLUMN rule_code INTEGER DEFAULT 0;

--step 2: update "treated as X" cards
UPDATE datas
SET rule_code = alias, alias = 0
WHERE id IN (
    SELECT id
	FROM datas
    WHERE NOT (type & 0x4000) AND alias != 0 AND abs(id - alias) >= 20
);

--step 3: update special cards
UPDATE datas
SET rule_code = alias, alias = 0
WHERE id = 5405695;

UPDATE datas
SET rule_code = 13331639
WHERE alias = 6218704;

--Add scale column
ALTER TABLE datas ADD COLUMN scale INTEGER DEFAULT 0;
UPDATE datas SET scale =  (level >> 24) & 0xff WHERE type & 0x1000000;
UPDATE datas SET level = level & 0xff WHERE type & 0x1000000;

--Add setcode2, setcode3, setcode4 columns
ALTER TABLE datas ADD COLUMN setcode2 INTEGER DEFAULT 0;
ALTER TABLE datas ADD COLUMN setcode3 INTEGER DEFAULT 0;
ALTER TABLE datas ADD COLUMN setcode4 INTEGER DEFAULT 0;
UPDATE datas SET setcode2 = 0x13a WHERE id IN (8512558, 55088578);
COMMIT;
