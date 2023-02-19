ALTER TABLE users RENAME COLUMN race_rating TO vs_rating;
ALTER TABLE users ADD COLUMN bt_rating integer NOT NULL DEFAULT 5000;
