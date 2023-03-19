ALTER TABLE users ADD COLUMN accepts_friends BOOLEAN NOT NULL DEFAULT TRUE;
ALTER TABLE users DROP COLUMN friend_suffix;

ALTER TABLE friendships DROP COLUMN confirmed;
