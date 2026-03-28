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
