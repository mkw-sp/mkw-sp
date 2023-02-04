ALTER TABLE users
    ALTER COLUMN friend_suffix TYPE smallint USING friend_suffix::smallint,
    ALTER COLUMN race_rating SET DEFAULT 5000;
